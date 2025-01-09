#include "MainWindow.h"

#include "com/ui/ComProfileWidget.h"
#include "com/ui/ComDeviceStatusWidget.h"
#include "com/ui/ComDeviceManualControl.h"
#include "com/ui/ComDeviceProportionalControl.h"
#include "com/ui/ComDeviceBiosignalControl.h"
#include "com/ui/ComDeviceMultiplayerControl.h"

#include "ev3/ui/EV3ManualControl.h"
#include "ev3/ui/EV3ProportionalControl.h"
#include "ev3/ui/EV3MultiplayerControl.h"
#include "ev3/ui/EV3BiosignalStateControl.h"

#include "ui/UICommon.h"

#include <QUrl>
#include <QDir>
#include <QLabel>
#include <QDebug>

#include <QEventLoop>
#include <QJsonObject>
#include <QTranslator>
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

    QString version = Common::appVersion(true);
    setWindowTitle(QCoreApplication::applicationName() + " " + version);

    m_settings = new Settings();

    m_com = new ComDevice();

    auto ev3ConnectedStatus = UICommon::Instance()->connectStatusWidget();
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

    auto btnEV3Connect = UICommon::iconedButton(":/resources/EV3.svg");
    btnEV3Connect->setCheckable(true);
    connect(btnEV3Connect, &QPushButton::toggled, this, [=](bool toggled) {
        if (toggled) m_ev3->searchAndConnect();
        else m_ev3->disconnect();
    });

    auto comProfileWidget = new ComProfileWidget(m_com);
    m_deviceWidgets[Device_COM] << comProfileWidget;

    m_tabs = new QTabWidget();
    m_tabs->addTab(comProfileWidget, QCoreApplication::translate("Generic", "Profile"));
    addTab(QCoreApplication::translate("Generic", "Manual"),
           new EV3ManualControl(m_ev3),
           new ComDeviceManualControl(m_com));

    addTab(QCoreApplication::translate("Generic", "Mental states"),
           new EV3BiosignalStateControl(m_ev3, m_settings),
           new ComDeviceBiosignalControl(m_com, m_settings));

    addTab(QCoreApplication::translate("Generic", "Meditation"),
           new EV3ProportionalControl(m_ev3, m_settings, true),
           new ComDeviceProportionalControl(m_com, m_settings, true), true);

    addTab(QCoreApplication::translate("Generic", "Concentration"),
           new EV3ProportionalControl(m_ev3, m_settings, false),
           new ComDeviceProportionalControl(m_com, m_settings, false),  false);

    addTab(QCoreApplication::translate("Generic", "Multiplayer"),
           new EV3MultiplayerControl(m_ev3, m_settings),
           new ComDeviceMultiplayerControl(m_com, m_settings));

    connect(m_tabs, &QTabWidget::currentChanged, [=](int index) {
        if (m_ev3) m_ev3->stopMotors();
        if (m_com) m_com->stop();
        m_controlState = (ControlState)index;
        m_neuroplayConnector->start(m_controlState == Multiplayer);
    });

    static QColor colorBlue = QColor(0x00CCFF);
    static QColor colorGray = QColor(0xCCCCCC);

    auto line1 = new QWidget();
    line1->setMaximumHeight(4);
    line1->setMinimumWidth(30);
    UICommon::ChangeBackground(line1, colorBlue);

    auto line2 = new QWidget();
    line2->setMaximumHeight(4);
    line2->setMinimumWidth(30);
    UICommon::ChangeBackground(line2, colorBlue);

    auto radioGroup = new QButtonGroup();
    auto radioDeviceEV3 = new QRadioButton("EV3");
    auto radioDeviceCOM = new QRadioButton("COM");
    radioGroup->addButton(radioDeviceEV3);
    radioGroup->addButton(radioDeviceCOM);
    connect(radioDeviceEV3, &QRadioButton::toggled, this, [=](bool ) {
        setDeviceMode(Device_EV3);
    });
    connect(radioDeviceCOM, &QRadioButton::toggled, this, [=](bool ) {
        setDeviceMode(Device_COM);
    });

    auto layoutDeviceSelection = new QVBoxLayout();
    layoutDeviceSelection->setContentsMargins(0,0,0,0);
    layoutDeviceSelection->setSpacing(0);
    layoutDeviceSelection->addWidget(radioDeviceEV3);
    layoutDeviceSelection->addWidget(radioDeviceCOM);

    static QIcon iconLink = QIcon(":/resources/link.svg");
    static QIcon iconLinkOff = QIcon(":/resources/link_off.svg");
    auto btnCanControl = UICommon::iconedButton("");
    btnCanControl->setCheckable(true);
    btnCanControl->setChecked(m_settings->bciLinked());

    auto func_bciLink = [=]() {
        auto toggled = btnCanControl->isChecked();
        btnCanControl->setIcon(toggled ? iconLink : iconLinkOff);
        m_canControl = toggled;
        UICommon::ChangeBackground(line1, toggled ? colorBlue : colorGray);
        UICommon::ChangeBackground(line2, toggled ? colorBlue : colorGray);

        btnCanControl->setToolTip(toggled ? tr("Turn BCI link off") : tr("Turn BCI link on"));
        m_settings->setBciLinked(toggled);
    };

    connect(btnCanControl, &QPushButton::toggled, func_bciLink);
    func_bciLink();

    auto widgetEV3connectionType = new QWidget();
    auto layoutEV3connectionType = new QVBoxLayout(widgetEV3connectionType);
    layoutEV3connectionType->setContentsMargins(0,0,0,0);
    layoutEV3connectionType->setSpacing(0);
    auto btnBluetooth = UICommon::iconedButton(":/resources/bluetooth.svg", QSize(20, 20), QSize(16, 16));
    btnBluetooth->setCheckable(true);
    auto btnWiFi = UICommon::iconedButton(":/resources/wifi.svg", QSize(20, 20), QSize(16, 16));
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

    auto btnNeuroPlayConnected = UICommon::Instance()->connectStatusWidget();
    auto btnNeuroPlay = UICommon::iconedButton(":/resources/neuroplay.png");
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

    auto btnNeurobotics = UICommon::flatButton("", QIcon(":/resources/neurobotics-logo.svg"), QCoreApplication::translate("Generic", "Visit neurobotics.ru"), QUrl("https://neurobotics.ru"));
    btnNeurobotics->setIconSize(QSize(68, 24));

    auto btnGithub = UICommon::flatButton("", QIcon(":/resources/github.svg"), QCoreApplication::translate("Generic", "Visit NeuroEV3 GitHub repo"), QUrl("https://github.com/neurobotics/neuroev3"));
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

    connect(m_neuroplayConnector, &NeuroPlayAppConnector::userBCI, this, [=](int userIndex, float meditation, float concentration, int biosignalState) {
        if (userIndex < 0 || userIndex >= m_userBCI.length()) return;

        m_userBCI[userIndex].meditation = meditation;
        m_userBCI[userIndex].concentration = concentration;
        m_userBCI[userIndex].biosignalState = biosignalState;

        if (userIndex == 0) control();
    }, Qt::QueuedConnection);

    connect(m_neuroplayConnector, &NeuroPlayAppConnector::userPairBCI, this, [=](QPair<float, float> meditation, QPair<float, float> concentration) {
        foreach (auto multiplayer, m_multiplayerControls) {
            multiplayer->setValues(meditation, concentration);
        }
    });

    m_neuroplayConnector->start(m_controlState == Multiplayer);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    setMinimumWidth(520);
#endif

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
        if (m_ev3) m_ev3->stopMotors();
        return;
    }

    switch (m_controlState) {
    case MentalState: {
        static qint64 bsTimestamp = 0;
        qint64 ts = QDateTime::currentMSecsSinceEpoch();
        // Limit to twice per second
        if ((ts - bsTimestamp) > 500) {
            bsTimestamp = ts;
            foreach (auto biosignalControl, m_biosignalStateControls) {
                biosignalControl->setCurrentState((int)m_userBCI[0].biosignalState);
            }
        }

    }
    break;

    case Meditation: {
        foreach (auto widget, m_meditationControls) {
            widget->setValue(m_userBCI[0].meditation);
        }
    }
    break;

    case Concentration: {
        foreach (auto widget, m_concentrationControls) {
            widget->setValue(m_userBCI[0].concentration);
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

void MainWindow::addTab(QString title, QWidget *widgetForEV3, QWidget *widgetForCOM)
{
    auto holder = new QWidget();
    holder->setContentsMargins(0,0,0,0);
    auto lay = new QVBoxLayout(holder);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(widgetForEV3);
    lay->addWidget(widgetForCOM);
    m_deviceWidgets[Device_EV3] << widgetForEV3;
    m_deviceWidgets[Device_COM] << widgetForCOM;

    auto func_addDeviceControl = [=](QWidget *w) {
        auto b = dynamic_cast<DeviceBiosignalStateControl*>(w);
        if (b) m_biosignalStateControls << b;

        auto m = dynamic_cast<DeviceMultiplayerControl*>(w);
        if (m) m_multiplayerControls << m;
    };

    func_addDeviceControl(widgetForCOM);
    func_addDeviceControl(widgetForEV3);

    m_tabs->addTab(holder, title);
}

void MainWindow::addTab(QString title, DeviceProportionalControl *widgetForEV3, DeviceProportionalControl *widgetForCOM, bool isMeditation)
{
    addTab(title, widgetForEV3, widgetForCOM);
    if (isMeditation) {
        if (widgetForEV3) m_meditationControls << widgetForEV3;
        if (widgetForCOM) m_meditationControls << widgetForCOM;
    } else {
        if (widgetForEV3) m_concentrationControls << widgetForEV3;
        if (widgetForCOM) m_concentrationControls << widgetForCOM;
    }
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
    QString version = Common::appVersion(true);
    QUrl url = "https://neurobotics.ru/repo/version.php?app="+QCoreApplication::applicationName()+"&version="+version + "&platform=" + Common::OS();

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
        auto btnNewVersion = UICommon::flatButton(QCoreApplication::translate("Generic", "New version available") + " " + version, QIcon(), QCoreApplication::translate("Generic", "Download new version"), QUrl(link));
        btnNewVersion->setStyleSheet("QPushButton { color: #159; text-decoration: underline } QPushButton:hover { color: #37B; }");
        return btnNewVersion;
    }
    return nullptr;
}
