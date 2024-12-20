#include "MainWindow.h"
#include "ev3/EV3_Motor.h"
#include "MotorsWidget.h"
#include "MotorsCoeffWidget.h"
#include "com/ComProfileWidget.h"
#include "com/ComDeviceStatusWidget.h"
#include "com/ComDeviceControl.h"

#include <QUrl>
#include <QDir>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QSlider>
#include <QSpinBox>
#include <QSysInfo>
#include <QCheckBox>
#include <QComboBox>
#include <QEventLoop>
#include <QJsonObject>
#include <QTranslator>
#include <QProgressBar>
#include <QRadioButton>
#include <QButtonGroup>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QNetworkAccessManager>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    autoDetectLanguage();

    m_deviceWidgets.insert(Device_EV3, QList<QWidget*>());
    m_deviceWidgets.insert(Device_COM, QList<QWidget*>());

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

    auto func_iconButton = [](QString iconPath, QSize size = QSize(42, 42), QSize iconSize = QSize(32, 32)) {
        auto btn = new QPushButton();
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(iconSize);
        btn->setFixedSize(size);
        btn->setContentsMargins(0,0,0,0);
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };

    m_com = new ComDevice();

    auto ev3ConnectedStatus = Common::Instance()->connectStatusWidget();
    m_deviceWidgets[Device_EV3] << ev3ConnectedStatus;

    auto comConnectedStatus = new ComDeviceStatusWidget(m_com);
    m_deviceWidgets[Device_COM] << comConnectedStatus;

    auto labelConnected = new QLabel("?");
    auto func_connected = [=]() {
        auto st = m_ev3->connectionState();
        labelConnected->setText(Common::EnumToString<EV3::ConnectionState>(st));
        ev3ConnectedStatus->setActive(st == EV3::ConnectionState::Connected);
    };
    m_deviceWidgets[Device_EV3] << labelConnected;

    m_ev3 = new EV3(m_settings->getConnectionType());
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
    currentMentalStateWidgetLayout->addWidget(new QLabel(tr("Current state:")));
    currentMentalStateWidgetLayout->addWidget(labelMentalState, 100, Qt::AlignLeft);

    auto bciStateMotors = new QWidget();
    auto bciStateMotorsLayout = new QVBoxLayout(bciStateMotors);
    for (int i = 1; i<=MAX_MENTAL_STATES; i++) {
        auto checkbox = new QCheckBox(tr("State") + " " + QString::number(i));
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

    auto comProfileWidget = new ComProfileWidget(m_com);
    m_deviceWidgets[Device_COM] << comProfileWidget;

    auto comControlWidget = new ComDeviceControl();
    comControlWidget->setComDevice(m_com);

    m_tabs = new QTabWidget();
    m_tabs->addTab(comProfileWidget, tr("Profile"));
    m_tabs->addTab(deviceConditionalWidget(manualMotors, comControlWidget), tr("Manual"));
    m_tabs->addTab(bciStateMotors, tr("Mental states"));

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

        m_tabs->addTab(bciMetaIndexMotors, tr(qPrintable(metaIndex.mid(0, 1).toUpper() + metaIndex.mid(1))));
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

    multipleGrid->addWidget(new QLabel(tr("User") + "1"), 0, 0, 1, 1, Qt::AlignLeft);
    multipleGrid->addWidget(new QLabel(tr("User") + "2"), 0, 3, 1, 1, Qt::AlignRight);

    multipleGrid->setColumnStretch(0, 100);
    multipleGrid->setColumnStretch(3, 100);

    auto multipleControlCombo = new QComboBox();
    auto multipleControlComboIndex = m_settings->getMultiplayerControl() == "meditation" ? 0 : 1;
    multipleControlCombo->addItem(tr("Meditation"));
    multipleControlCombo->addItem(tr("Concentration"));
    multipleControlCombo->setCurrentIndex(multipleControlComboIndex);

    connect(multipleControlCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=]() {
        m_settings->setMultiplayerControl(multipleControlCombo->currentIndex() == 0 ? "meditation" : "concentration");
    });

    multipleGrid->addWidget(new QLabel(tr("Control")), 0, 1, 1, 2, Qt::AlignCenter);
    multipleGrid->addWidget(multipleControlCombo, 1, 1, 1, 2, Qt::AlignCenter);

    multiplayerWidget->addWidgetOnTop(multipl);

    multipleGrid->addWidget(progressMeditationMix1,     3, 0, 1, 2, Qt::AlignLeft);
    multipleGrid->addWidget(progressMeditationMix2,     3, 2, 1, 2, Qt::AlignLeft);

    multipleGrid->addWidget(progressConcentrationMix1,     4, 0, 1, 2, Qt::AlignLeft);
    multipleGrid->addWidget(progressConcentrationMix2,     4, 2, 1, 2, Qt::AlignLeft);

    m_tabs->addTab(multiplayerWidget, tr("Multiplayer"));

    connect(m_tabs, &QTabWidget::currentChanged, [=](int index) {
        m_multiplayer = index == m_tabs->count()-1;
        m_ev3->stopMotors();
        m_controlState = (ControlState)index;
        m_neuroplayConnector->start(m_multiplayer);
    });

    static QColor colorBlue = QColor(0x00CCFF);
    static QColor colorGray = QColor(0xCCCCCC);

    auto line1 = new QWidget();
    line1->setMaximumHeight(4);
    line1->setMinimumWidth(30);
    ChangeBackground(line1, colorBlue);

    auto line2 = new QWidget();
    line2->setMaximumHeight(4);
    line2->setMinimumWidth(30);
    ChangeBackground(line2, colorBlue);

    auto radioGroup = new QButtonGroup();
    auto radioDeviceEV3 = new QRadioButton("EV3");
    auto radioDeviceCOM = new QRadioButton("COM");
    radioGroup->addButton(radioDeviceEV3);
    radioGroup->addButton(radioDeviceCOM);
    connect(radioDeviceEV3, &QRadioButton::toggled, this, [=](bool on) {
        setDeviceMode(Device_EV3);
    });
    connect(radioDeviceCOM, &QRadioButton::toggled, this, [=](bool on) {
        setDeviceMode(Device_COM);
    });

    auto layoutDeviceSelection = new QVBoxLayout();
    layoutDeviceSelection->setContentsMargins(0,0,0,0);
    layoutDeviceSelection->setSpacing(0);
    layoutDeviceSelection->addWidget(radioDeviceEV3);
    layoutDeviceSelection->addWidget(radioDeviceCOM);

    static QIcon iconLink = QIcon(":/resources/link.svg");
    static QIcon iconLinkOff = QIcon(":/resources/link_off.svg");
    auto btnCanControl = func_iconButton("");
    btnCanControl->setCheckable(true);
    btnCanControl->setChecked(true);
    btnCanControl->setIcon(iconLink);

    connect(btnCanControl, &QPushButton::toggled, [=](bool toggled) {
        btnCanControl->setIcon(toggled ? iconLink : iconLinkOff);
        m_canControl = toggled;
        ChangeBackground(line1, toggled ? colorBlue : colorGray);
        ChangeBackground(line2, toggled ? colorBlue : colorGray);
    });

    auto widgetEV3connectionType = new QWidget();
    auto layoutEV3connectionType = new QVBoxLayout(widgetEV3connectionType);
    layoutEV3connectionType->setContentsMargins(0,0,0,0);
    layoutEV3connectionType->setSpacing(0);
    auto btnBluetooth = func_iconButton(":/resources/bluetooth.svg", QSize(20, 20), QSize(16, 16));
    btnBluetooth->setCheckable(true);
    auto btnWiFi = func_iconButton(":/resources/wifi.svg", QSize(20, 20), QSize(16, 16));
    btnWiFi->setCheckable(true);

    layoutEV3connectionType->addWidget(btnWiFi);
    layoutEV3connectionType->addWidget(btnBluetooth);

    auto func_setMode = [=](EV3::ConnectionType type) {
        m_settings->setConnectionType(type);
        m_ev3->setConnectionType(type);
        btnBluetooth->setChecked(type == EV3::Bluetooth);
        btnWiFi->setChecked(type == EV3::WiFi);
        btnEV3Connect->setChecked(false);
        btnEV3Connect->setCheckable(type != EV3::Bluetooth);
        btnEV3Connect->setFlat(type == EV3::Bluetooth);
    };

    connect(btnBluetooth, &QRadioButton::clicked, [=]() {
        func_setMode(EV3::Bluetooth);
    });

    connect(btnWiFi, &QRadioButton::clicked, [=]() {
        func_setMode(EV3::WiFi);
    });

    func_setMode(m_settings->getConnectionType());

    auto btnNeuroPlayConnected = Common::Instance()->connectStatusWidget();
    auto btnNeuroPlay = func_iconButton(":/resources/neuroplay.png");
    btnNeuroPlay->setFlat(true);

    m_deviceWidgets[Device_EV3] << btnEV3Connect;

    auto headerLayout = new QHBoxLayout();
    headerLayout->addLayout(layoutDeviceSelection);
    headerLayout->addWidget(btnEV3Connect);
    headerLayout->addLayout(layoutEV3connectionType);
    headerLayout->addWidget(ev3ConnectedStatus);
    headerLayout->addWidget(comConnectedStatus);
    headerLayout->addWidget(labelConnected);
    headerLayout->addWidget(line1, 100);
    headerLayout->addWidget(btnCanControl, Qt::AlignCenter);
    headerLayout->addWidget(line2, 100);
    headerLayout->addWidget(btnNeuroPlayConnected);
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
    grid->addWidget(m_tabs, 100);
    grid->addLayout(bottomLayout);

    m_neuroplayConnector = new NeuroPlayAppConnector();
    connect(m_neuroplayConnector, &NeuroPlayAppConnector::connected, [=]() {
        m_state = EV3::ConnectionState::Connected;
        btnNeuroPlayConnected->setActive(true);
    });
    connect(m_neuroplayConnector, &NeuroPlayAppConnector::disconnected, [=]() {
        m_state = EV3::ConnectionState::Disconnected;
        btnNeuroPlayConnected->setActive(false);
    });

    connect(m_neuroplayConnector, &NeuroPlayAppConnector::userBCI, this, [=](int userIndex, float meditation, float concentration, int mentalState) {
        if (userIndex < 0 || userIndex >= m_userBCI.length()) return;

        m_userBCI[userIndex].meditation = meditation;
        m_userBCI[userIndex].concentration = concentration;
        m_userBCI[userIndex].mentalState = mentalState;

        if (!m_multiplayer) {
            progressMeditation->setValue(m_userBCI[0].meditation);
            progressConcentration->setValue(m_userBCI[0].concentration);
            labelMentalState->setText(QString::number((int)m_userBCI[0].mentalState));

            control();
        } else if (userIndex < 2) {
            if (userIndex == 0) {
                progressMeditationU1->setValue(m_userBCI[0].meditation);
                progressConcentrationU1->setValue(m_userBCI[0].concentration);
            }

            if (userIndex == 1) {
                progressMeditationU2->setValue(m_userBCI[1].meditation);
                progressConcentrationU2->setValue(m_userBCI[1].concentration);

                auto med = m_userBCI[1].meditation - m_userBCI[0].meditation;
                auto con = m_userBCI[1].concentration - m_userBCI[0].concentration;

                progressMeditationMix1->setValue(med < 0 ? -med : 0);
                progressMeditationMix2->setValue(med > 0 ? med : 0);

                progressConcentrationMix1->setValue(con < 0 ? -con : 0);
                progressConcentrationMix2->setValue(con > 0 ? con : 0);

                control();
            }
        }
    }, Qt::QueuedConnection);

    m_neuroplayConnector->start(m_multiplayer);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    setMinimumWidth(650);
#endif

    //TODO: Apply settings
    if (m_settings->ev3Mode()) {
        radioDeviceEV3->setChecked(true);
    } else {
        radioDeviceCOM->setChecked(true);
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    if (m_ev3) m_ev3->disconnect();
}

MainWindow::~MainWindow()
{
    if (m_ev3) m_ev3->disconnect();
    if (m_neuroplayConnector) m_neuroplayConnector->stop();
}

void MainWindow::control()
{
    if (m_controlState != Manual && !m_canControl) {
        m_ev3->stopMotors();
        return;
    }

    if (m_multiplayer) {
        bool isMed = m_settings->getMultiplayerControl() == "meditation";
        float val = isMed ? m_userBCI[0].meditation - m_userBCI[1].meditation : m_userBCI[0].concentration - m_userBCI[1].concentration;
        for (int i = 1; i <= MAX_MOTORS; i++) {
            if (m_settings->getMetaIndexDriveEnabled(MULTIPLAYER, i)) {
                m_ev3->motor(i)->setPower(m_settings->getMetaIndexDriveCoeff(MULTIPLAYER, i) * val);
            } else {
                m_ev3->motor(i)->setPower(0);
            }
        }
        return;
    }

    switch (m_controlState) {
    case MentalState: {
        auto state = m_userBCI[0].mentalState;
        if (m_settings->getMentalStateEnabled(state)) {
            auto drives = m_settings->getMentalStateDrives(m_userBCI[0].mentalState);
            for (int i = 0; i < drives.length(); i++) {
                m_ev3->motor(i + 1)->setPower(drives[i]);
            }
        } else {
            m_ev3->stopMotors();
        }
    }
    break;

    case Meditation:
    case Concentration: {
        QString metaIndex = m_controlState == Meditation ? MEDITATION : CONCENTRATION;
        double metaIndexValue = m_controlState == Meditation ? m_userBCI[0].meditation : m_userBCI[0].concentration;
        for (int i = 1; i <= MAX_MOTORS; i++) {
            int power = m_settings->getMetaIndexDriveEnabled(metaIndex, i) ? m_settings->getMetaIndexDriveCoeff(metaIndex, i) * metaIndexValue : 0;
            m_ev3->motor(i)->setPower(power);
        }
    }
    break;

    default: break;
    }
}

void MainWindow::setDeviceMode(DeviceMode mode)
{
    m_tabs->setTabVisible(0, mode == Device_COM);

    if (mode != Device_EV3) {
        if (m_ev3) {
            m_ev3->disconnect();
        }
        m_tabs->setCurrentIndex(0);

        m_com->setEnabled(true);
    } else {
        m_com->setEnabled(false);
    }

    m_deviceMode = mode;

    foreach (auto deviceMode, m_deviceWidgets.keys()) {
        bool visible = deviceMode == mode;
        foreach (auto widget, m_deviceWidgets[deviceMode]) {
            if (widget) widget->setVisible(visible);
        }
    }

    m_settings->setEV3mode(mode == Device_EV3);
}

QWidget *MainWindow::deviceConditionalWidget(QWidget *widgetForEV3, QWidget *widgetForCOM)
{
    auto holder = new QWidget();
    holder->setContentsMargins(0,0,0,0);
    auto lay = new QVBoxLayout(holder);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(widgetForEV3);
    lay->addWidget(widgetForCOM);
    m_deviceWidgets[Device_EV3] << widgetForEV3;
    m_deviceWidgets[Device_COM] << widgetForCOM;
    return holder;
}

QString MainWindow::OS()
{
    static QString _os = "";
    if (_os.isEmpty()) {
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

void MainWindow::ChangeBackground(QWidget *w, QColor color)
{
    QPalette pal(w->palette());
    pal.setColor(QPalette::Window, color);
    w->setAutoFillBackground(true);
    w->setPalette(pal);
}

QString MainWindow::autoDetectLanguage(QString settingsFile)
{
    QString lang = "en-US";
    QString locale = QLocale::system().name();

    if (locale.startsWith("ru")) lang = "ru-RU";

    if (!settingsFile.isEmpty() && QFile::exists(settingsFile)) {
        QFile sets(settingsFile);
        if (sets.open(QIODevice::ReadOnly)) {
            QString allSets = sets.readAll();
            if (allSets.contains("<Language>en_US</Language>")) {
                lang = "en-US";
            } else if (allSets.contains("<Language>ru_RU</Language>")) {
                lang = "ru-RU";
            }
        }
    }

    auto translator = new QTranslator(qApp);
    auto qm = ":/resources/translations/NeuroEV3_" + lang + ".qm";
    if (translator->load(qm)) {
        qApp->installTranslator(translator);
    }
    return lang;
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
