#include "MainWindow.h"
#include "ev3/EV3.h"
#include "MotorsWidget.h"
#include "MotorsCoeffWidget.h"
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTabWidget>
#include <QCheckBox>
#include <QDebug>
#include <QProgressBar>
#include <QCoreApplication>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QDesktopServices>
#include <QSysInfo>
#include <QDir>
#include <QStandardPaths>
#include <QJsonArray>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    for (int i = 0; i<4; i++) m_userBCI << UserBCI();

    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(folder);
    if (!dir.exists()) {
        dir.mkdir(folder);
    }
    QDir::setCurrent(folder);

    QString version = appVersion(true);
    setWindowTitle(QCoreApplication::applicationName() + " " + version);

    m_settings = new Settings();

    auto func_iconButton = [](QString iconPath, QSize size = QSize(24, 24)) {
        auto btn = new QPushButton();
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(size);
        btn->setStyleSheet("padding: 2px");
        btn->setContentsMargins(0,0,0,0);
        btn->setFixedSize(size.width() + 6, size.height() + 6);
        return btn;
    };

    auto btnEV3Connected = Common::Instance()->connectStatusWidget();

    auto labelConnected = new QLabel("?");
    auto func_connected = [=]() {
        auto st = m_ev3->connectionState();
        labelConnected->setText(EnumToString<EV3::ConnectionState>(st));
        btnEV3Connected->setActive(st == EV3::ConnectionState::Connected);
    };

    m_ev3 = new EV3();
    connect(m_ev3, &EV3::connectionStateChanged, this, func_connected, Qt::QueuedConnection);

    auto btnEV3Connect = func_iconButton(":/resources/EV3.svg");
    btnEV3Connect->setCheckable(true);
    connect(btnEV3Connect, &QPushButton::toggled, this, [=](bool toggled) {
        if (toggled) m_ev3->searchAndConnect();
        else m_ev3->disconnect();
    });

    auto manualMotors = new MotorsWidget();
    connect(manualMotors, &MotorsWidget::motorValueChangeRequest, [=](int motorIndex, int value) {
        auto motor = m_ev3->motor(motorIndex);
        if (!motor) return;
        if (value == 0) {
            motor->stop();
        } else {
            motor->setPower(value);
        }
    });

    auto func_progress = [](QString color) {
        auto progress = new QProgressBar();
        progress->setRange(0, 100);
        progress->setStyleSheet("QProgressBar { text-align: center; border: 2px solid grey; border-radius: 5px; background-color: #FFF; } "
                              "QProgressBar::chunk { background-color: " + color + "; padding: 4px; }");
        return progress;
    };

    auto progressMeditation = func_progress("#7c2");
    auto progressConcentration = func_progress("#C30");

    auto labelMentalState = new QLabel();
    labelMentalState->setStyleSheet("font-weight: bold");

    auto currentMentalStateWidget = new QWidget();
    auto currentMentalStateWidgetLayout = new QHBoxLayout(currentMentalStateWidget);
    currentMentalStateWidgetLayout->setContentsMargins(0,0,0,0);
    currentMentalStateWidgetLayout->addWidget(new QLabel("Текущее состояние:"));
    currentMentalStateWidgetLayout->addWidget(labelMentalState, 100, Qt::AlignLeft);

    auto bciStateMotors = new QWidget();
    auto bciStateMotorsLayout = new QVBoxLayout(bciStateMotors);
    for (int i = 1; i<=MAX_MENTAL_STATES; i++) {
        auto checkbox = new QCheckBox("State " + QString::number(i));
        checkbox->setChecked(m_settings->getMentalStateEnabled(i));
        connect(checkbox, &QCheckBox::toggled, [=](bool toggled) {
            m_settings->setMentalStateEnabled(i, toggled);
        });

        auto motors = new MotorsWidget();
        motors->layout()->setContentsMargins(0,0,0,0);
        for (int j = 1; j<=MAX_MOTORS; j++) {
            motors->setMotorValue(j, m_settings->getMentalStateDrive(i, j));
        }

        connect(motors, &MotorsWidget::motorValueChangeRequest, [=](int motorIndex, int value) {
            m_settings->setMentalStateDrive(i, motorIndex, value);
        });

        auto lay = new QVBoxLayout();
        lay->addWidget(checkbox);
        lay->addWidget(motors);

        bciStateMotorsLayout->addLayout(lay);
    }
    bciStateMotorsLayout->insertWidget(0, currentMentalStateWidget);

    auto tabs = new QTabWidget();
    tabs->addTab(manualMotors, "Manual");
    tabs->addTab(bciStateMotors, "Mental states");

    auto metaIndexes = QStringList { MEDITATION, CONCENTRATION };
    foreach(auto metaIndex, metaIndexes) {
        auto bciMetaIndexMotors = new MotorsCoeffWidget();
        for (int i = 1; i<=MAX_MOTORS; i++) {
            bciMetaIndexMotors->setMotorCoeff(i, m_settings->getMetaIndexDriveCoeff(metaIndex, i));
            bciMetaIndexMotors->setMotorEnabled(i, m_settings->getMetaIndexDriveEnabled(metaIndex, i));
        }
        connect(bciMetaIndexMotors, &MotorsCoeffWidget::motorEnabledChanged, [=](int motorIndex, bool enabled) {
            m_settings->setMetaIndexDriveEnabled(metaIndex, motorIndex, enabled);
        });
        connect(bciMetaIndexMotors, &MotorsCoeffWidget::motorCoeffChanged, [=](int motorIndex, double coeff) {
            m_settings->setMetaIndexDriveCoeff(metaIndex, motorIndex, coeff);
        });

        bciMetaIndexMotors->addWidgetOnTop(metaIndex == MEDITATION ? progressMeditation : progressConcentration);

        tabs->addTab(bciMetaIndexMotors, metaIndex.mid(0, 1).toUpper() + metaIndex.mid(1));
    }

    auto multiplayerWidget = new MotorsCoeffWidget();

    for (int i = 1; i<=MAX_MOTORS; i++) {
        multiplayerWidget->setMotorCoeff(i, m_settings->getMetaIndexDriveCoeff(MULTIPLAYER, i));
        multiplayerWidget->setMotorEnabled(i, m_settings->getMetaIndexDriveEnabled(MULTIPLAYER, i));
    }
    connect(multiplayerWidget, &MotorsCoeffWidget::motorEnabledChanged, [=](int motorIndex, bool enabled) {
        m_settings->setMetaIndexDriveEnabled(MULTIPLAYER, motorIndex, enabled);
    });
    connect(multiplayerWidget, &MotorsCoeffWidget::motorCoeffChanged, [=](int motorIndex, double coeff) {
        m_settings->setMetaIndexDriveCoeff(MULTIPLAYER, motorIndex, coeff);
    });

    auto progressMeditationU1 = func_progress("#7c2");
    auto progressConcentrationU1 = func_progress("#C30");
    auto progressMeditationU2 = func_progress("#7c2");
    progressMeditationU2->setLayoutDirection(Qt::RightToLeft);
    auto progressConcentrationU2 = func_progress("#C30");
    progressConcentrationU2->setLayoutDirection(Qt::RightToLeft);


    auto progressMeditationMix1 = func_progress("#7c2");
    progressMeditationMix1->setLayoutDirection(Qt::RightToLeft);
    auto progressMeditationMix2 = func_progress("#7c2");
    auto progressConcentrationMix1 = func_progress("#C30");
    progressConcentrationMix1->setLayoutDirection(Qt::RightToLeft);
    auto progressConcentrationMix2 = func_progress("#C30");

    auto multipl = new QWidget();
    auto multipleGrid = new QGridLayout(multipl);

    multipleGrid->addWidget(progressMeditationU1,     1, 0, 1, 1, Qt::AlignLeft);
    multipleGrid->addWidget(progressConcentrationU1,  2, 0, 1, 1, Qt::AlignLeft);
    multipleGrid->addWidget(progressMeditationU2,     1, 3, 1, 1, Qt::AlignLeft);
    multipleGrid->addWidget(progressConcentrationU2,  2, 3, 1, 1, Qt::AlignLeft);

    multipleGrid->addWidget(new QLabel("User1"), 0, 0, 1, 1, Qt::AlignLeft);
    multipleGrid->addWidget(new QLabel("User2"), 0, 3, 1, 1, Qt::AlignRight);

    multipleGrid->setColumnStretch(0, 100);
    multipleGrid->setColumnStretch(3, 100);

    auto multipleControlCombo = new QComboBox();
    auto multipleControlComboIndex = m_settings->getMultiplayerControl() == "meditation" ? 0 : 1;
    multipleControlCombo->addItem(tr("Meditation"));
    multipleControlCombo->addItem(tr("Concentration"));
    multipleControlCombo->setCurrentIndex(multipleControlComboIndex);

    connect(multipleControlCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=]()
    {
       m_settings->setMultiplayerControl(multipleControlCombo->currentIndex() == 0 ? "meditation" : "concentration");
    });

    multipleGrid->addWidget(new QLabel("Control"), 0, 1, 1, 2, Qt::AlignCenter);
    multipleGrid->addWidget(multipleControlCombo, 1, 1, 1, 2, Qt::AlignCenter);

    multiplayerWidget->addWidgetOnTop(multipl);

    multipleGrid->addWidget(progressMeditationMix1,     3, 0, 1, 2, Qt::AlignLeft);
    multipleGrid->addWidget(progressMeditationMix2,     3, 2, 1, 2, Qt::AlignLeft);

    multipleGrid->addWidget(progressConcentrationMix1,     4, 0, 1, 2, Qt::AlignLeft);
    multipleGrid->addWidget(progressConcentrationMix2,     4, 2, 1, 2, Qt::AlignLeft);

    tabs->addTab(multiplayerWidget, "Multiplayer");

    connect(tabs, &QTabWidget::currentChanged, [=](int index) {
        m_multiplayer = index == tabs->count()-1;
        qDebug() << "MULTIPLE" << m_multiplayer;

        m_ev3->stopMotors();
        m_controlState = (ControlState)index;
    });



    static QIcon iconLink = QIcon(":/resources/link.svg");
    static QIcon iconLinkOff = QIcon(":/resources/link_off.svg");
    auto btnCanControl = new QPushButton();
    btnCanControl->setCheckable(true);
    btnCanControl->setChecked(true);
    btnCanControl->setIcon(iconLink);
    btnCanControl->setFixedSize(30, 30);
    btnCanControl->setIconSize(QSize(24, 24));

    connect(btnCanControl, &QPushButton::toggled, [=](bool toggled) {
        btnCanControl->setIcon(toggled ? iconLink : iconLinkOff);
        m_canControl = toggled;
    });

    auto btnNeuroPlayConnected = Common::Instance()->connectStatusWidget();
    auto btnNeuroPlay = func_iconButton(":/resources/neuroplay.png");
    btnNeuroPlay->setFlat(true);

    auto headerLayout = new QHBoxLayout();
    headerLayout->addWidget(btnEV3Connect);
    headerLayout->addWidget(btnEV3Connected);
    headerLayout->addWidget(labelConnected, 100);
    headerLayout->addWidget(btnCanControl, Qt::AlignCenter);
    headerLayout->addWidget(btnNeuroPlayConnected, 100, Qt::AlignRight);
    headerLayout->addWidget(btnNeuroPlay);

    auto btnNeurobotics = flatButton("", QIcon(":/resources/neurobotics-logo.svg"), tr("Visit neurobotics.ru"), QUrl("https://neurobotics.ru"));
    btnNeurobotics->setIconSize(QSize(68, 24));

    auto btnGithub = flatButton("", QIcon(":/resources/github.svg"), tr("Visit NeuroEV3 GitHub repo"), QUrl("https://github.com/neurobotics/neuroev3"));
    btnGithub->setIconSize(QSize(24, 24));

    auto bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(btnNeurobotics, 0, Qt::AlignLeft);
    auto btnNewVersion = newVersionButton();
    if (btnNewVersion) {
        bottomLayout->addWidget(btnNewVersion, 100, Qt::AlignCenter);
    }
    bottomLayout->addWidget(btnGithub, 0, Qt::AlignRight);

    auto centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    auto grid = new QVBoxLayout(centralWidget);
    grid->addLayout(headerLayout);
    grid->addWidget(tabs, 100);
    grid->addLayout(bottomLayout);

    socket = new QWebSocket();
    connect(socket, &QWebSocket::connected, [=]() {
        m_state = EV3::ConnectionState::Connected;
        btnNeuroPlayConnected->setActive(true);
    });
    connect(socket, &QWebSocket::disconnected, [=]() {
        m_state = EV3::ConnectionState::Disconnected;
        btnNeuroPlayConnected->setActive(false);
    });

    auto func_parseBCI = [](QJsonObject resp, UserBCI &bci)
    {
        auto med = resp["meditation"];
        if (!med.isNull()) {
            bci.meditation = med.toDouble();

        }

        auto con = resp["concentration"];
        if (!con.isNull()) {
            bci.concentration = con.toDouble();

        }

        auto st = resp["mental_state"];
        if (!st.isNull()) {
            bci.mentalState = st.toInt();

        }
    };

    connect(socket, &QWebSocket::textMessageReceived, this, [=](QString txt) {
        QJsonDocument json = QJsonDocument::fromJson(txt.toUtf8());
        QJsonObject resp = json.object();

        if (!m_multiplayer) {
            QString cmd = resp["command"].toString();

            func_parseBCI(resp, m_userBCI[0]);

            progressMeditation->setValue(m_userBCI[0].meditation);
            progressConcentration->setValue(m_userBCI[0].concentration);
            labelMentalState->setText(QString::number((int)m_userBCI[0].mentalState));

            control();
        }
        else
        {
             QString cmd = resp["command"].toString();

             auto devs = resp["results"];
             if (devs.isArray())
             {
                 auto devices = devs.toArray();
                 int index = 0;
                 foreach (auto dev, devices)
                 {
                     if (dev.isObject())
                     {
                        func_parseBCI(dev.toObject(), m_userBCI[index]);
                     }
                     index ++;
                     if (index >= m_userBCI.length()) break;
                 }
             }

             qDebug() << devs;

             progressMeditationU1->setValue(m_userBCI[0].meditation);
             progressConcentrationU1->setValue(m_userBCI[0].concentration);

             progressMeditationU2->setValue(m_userBCI[1].meditation);
             progressConcentrationU2->setValue(m_userBCI[1].concentration);

             auto med = m_userBCI[1].meditation - m_userBCI[0].meditation;
             auto con = m_userBCI[1].concentration - m_userBCI[0].concentration;

             progressMeditationMix1->setValue(med < 0 ? -med : 0);
             progressMeditationMix2->setValue(med > 0 ? med : 0);

             progressConcentrationMix1->setValue(con < 0 ? -con : 0);
             progressConcentrationMix2->setValue(con > 0 ? con : 0);

        }
    }, Qt::QueuedConnection);

    auto timer = new QTimer();
    timer->setInterval(200);
    connect(timer, &QTimer::timeout, [=]() {
        if (socket->state() != QAbstractSocket::ConnectedState) {
            socket->open(QUrl("ws://localhost:1336"));
        } else {
            socket->sendTextMessage(m_multiplayer ? "bcis" : "bci");
        }
    });
    timer->start();

#ifdef OS_DESKTOP
    setMinimumWidth(500);
#endif
}

MainWindow::~MainWindow()
{
    m_ev3->disconnect();
}

void MainWindow::control()
{
    if (m_controlState != Manual && !m_canControl) {
        m_ev3->stopMotors();
        return;
    }

    if (m_multiplayer)
    {
        bool isMed = m_settings->getMultiplayerControl() == "meditation";

        bool val = isMed ? m_userBCI[0].meditation - m_userBCI[1].meditation : m_userBCI[0].concentration - m_userBCI[1].concentration;

        for (int i = 1; i <= MAX_MOTORS; i++) {
            if (m_settings->getMetaIndexDriveEnabled(MULTIPLAYER, i)) {
                m_ev3->motor(i)->setPower(m_settings->getMetaIndexDriveCoeff(MULTIPLAYER, i) * val);
            } else {
                m_ev3->motor(i)->setPower(0);
            }
        }

        return;
    }

    switch (m_controlState)
    {
        case MentalState: {
            auto drives = m_settings->getMentalStateDrives(m_userBCI[0].mentalState);
            for (int i = 0; i < drives.length(); i++) {
                m_ev3->motor(i + 1)->setPower(drives[i]);
            }
        }
        break;

        case Meditation:
        case Concentration:
        {
            QString metaIndex = m_controlState == Meditation ? MEDITATION : CONCENTRATION;
            double metaIndexValue = m_controlState == Meditation ? m_userBCI[0].meditation : m_userBCI[0].concentration;
            for (int i = 1; i <= MAX_MOTORS; i++) {
                if (m_settings->getMetaIndexDriveEnabled(metaIndex, i)) {
                    m_ev3->motor(i)->setPower(m_settings->getMetaIndexDriveCoeff(metaIndex, i) * metaIndexValue);
                } else {
                    m_ev3->motor(i)->setPower(0);
                }
            }
        }
        break;

        default: break;
    }
}

QString MainWindow::OS()
{
    static QString _os = "";
    if (_os.isEmpty())
    {
        auto os = QSysInfo::productType().toLower();

        if (os.contains("windows") || os.contains("winrt")) _os = "windows";

        else if (os.contains("android")) _os = "android";

        else if (os.contains("osx") || os.contains("mac")) _os = "macos";

        else if (os.contains("ios")) _os = "ios";
        else if (os.contains("tvos")) _os = "tvos";
        else if (os.contains("watchos")) _os = "watchos";
        else if (os.contains("ubuntu") ||
                 os.contains("linux") ||
                 os.contains("manjaro") ||
                 os.contains("centos") ||
                 os.contains("fedora") ||
                 os.contains("raspberry"))
        {
            _os = "linux";
        }
        else _os = "unix";
    }
    return _os;
}

QWidget *MainWindow::newVersionButton()
{
    QString version = appVersion(true);
    QUrl url = "https://neurobotics.ru/repo/version.php?app="+QCoreApplication::applicationName()+"&version="+version + "&platform=" + OS();

    QEventLoop connection_loop;
    QNetworkAccessManager networkManager;
    QNetworkRequest networkRequest;
    networkManager.connect(&networkManager, SIGNAL(finished(QNetworkReply*)), &connection_loop, SLOT(quit()));
    networkRequest.setUrl( url );
    auto reply = networkManager.get(networkRequest );
    connection_loop.exec();
    reply->deleteLater();
    QByteArray bytes = reply->readAll();

    QJsonDocument d = QJsonDocument::fromJson(bytes);
    QJsonObject o = d.object();

    if (o.value("valid").toBool() == true) {
        const QString link = o.value("link").toString();
        const QString version = o.value("version").toString();
        auto btnNewVersion = flatButton(tr("New version available") + " " + version, QIcon(), tr("Download new version"), QUrl(link));
        btnNewVersion->setStyleSheet("QPushButton { color: #159; text-decoration: underline } QPushButton:hover { color: #37B; }");
        return btnNewVersion;
    }
    return nullptr;
}

QPushButton *MainWindow::flatButton(QString text, QIcon icon, QString tooltip, QUrl url)
{
    auto btn = new QPushButton(icon, text);
    btn->setFlat(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setToolTip(tooltip);
    if (!url.isEmpty()) {
        connect(btn, &QPushButton::clicked, [=]() {
            QDesktopServices::openUrl(url);
        });
    }
    return btn;
}

QString MainWindow::appVersion(bool withBuild)
{
    QStringList versions = QCoreApplication::applicationVersion().split(".");
    int m = withBuild ? 3 : 2;
    while (versions.length() > m) {
        versions.removeLast();
    }
    return versions.join(".");
}
