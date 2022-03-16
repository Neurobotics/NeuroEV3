#include "MainWindow.h"
#include "EV3.h"
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QSlider>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTabWidget>
#include "MotorsWidget.h"
#include "MotorsCoeffWidget.h"
#include <QCheckBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_settings = new Settings();

    auto func_iconButton = [](QString iconPath, QSize size = QSize(24, 24)) {
        auto btn = new QPushButton();
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(size);
        btn->setStyleSheet("padding: 2px");
        btn->setContentsMargins(0,0,0,0);
        return btn;
    };

    static QIcon iconConnected = QIcon(":/resources/connected.svg");
    static QIcon iconDisconnected = QIcon(":/resources/disconnected.svg");
    auto func_connectedButton = [=](QSize size = QSize(24, 24)) {
        auto label = new QLabel();
        label->setFixedSize(size);
        label->setPixmap(iconDisconnected.pixmap(size));

        connect(label, &QLabel::objectNameChanged, [=]() {
            label->setPixmap((label->objectName() == "connected" ? iconConnected : iconDisconnected).pixmap(size));
        });
        return label;
    };

    auto btnEV3Connected = func_connectedButton();

    auto labelConnected = new QLabel("?");
    auto func_connected = [=]() {
        auto st = m_ev3->connectionState();
        labelConnected->setText(EnumToString<EV3::ConnectionState>(st));
        btnEV3Connected->setObjectName(st == EV3::ConnectionState::Connected ? "connected" : "");
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

    auto centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    auto headerLayout = new QHBoxLayout();
    // headerLayout->addWidget(func_iconButton(":/resources/EV3.svg"));
    headerLayout->addWidget(btnEV3Connect);
    headerLayout->addWidget(btnEV3Connected);
    headerLayout->addWidget(labelConnected, 100);

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
        tabs->addTab(bciMetaIndexMotors, metaIndex);
    }

    connect(tabs, &QTabWidget::currentChanged, [=](int index) {
        m_ev3->stopMotors();
        m_controlState = (ControlState)index;
    });

    auto grid = new QVBoxLayout(centralWidget);
    grid->addLayout(headerLayout);
    grid->addWidget(tabs);

    auto btnNeuroPlayConnected = func_connectedButton();
    auto btnNeuroPlay = func_iconButton(":/resources/neuroplay.png");
    btnNeuroPlay->setFlat(true);
    headerLayout->addWidget(btnNeuroPlayConnected);
    headerLayout->addWidget(btnNeuroPlay);

    socket = new QWebSocket();
    connect(socket, &QWebSocket::connected, [=]() {
        m_state = EV3::ConnectionState::Connected;
        btnNeuroPlayConnected->setObjectName("connected");
    });
    connect(socket, &QWebSocket::disconnected, [=]() {
        m_state = EV3::ConnectionState::Disconnected;
        btnNeuroPlayConnected->setObjectName("");
    });
    connect(socket, &QWebSocket::textMessageReceived, this, [=](QString txt) {
        QJsonDocument json = QJsonDocument::fromJson(txt.toUtf8());
        QJsonObject resp = json.object();
        QString cmd = resp["command"].toString();

        auto med = resp["meditation"];
        if (!med.isNull()) {
            m_meditation = med.toDouble();
        }

        auto con = resp["concentration"];
        if (!con.isNull()) {
            m_concentration = con.toDouble();
        }

        auto st = resp["mental_state"];
        if (!st.isNull()) {
            m_mentalState = st.toInt();
        }

        control();
    });

    auto timer = new QTimer();
    timer->setInterval(200);
    connect(timer, &QTimer::timeout, [=]() {
        if (socket->state() != QAbstractSocket::ConnectedState) {
            socket->open(QUrl("ws://localhost:1336"));
        } else {
            socket->sendTextMessage("bci");
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
    switch (m_controlState)
    {
        case MentalState: {
            auto drives = m_settings->getMentalStateDrives(m_mentalState);
            for (int i = 0; i < drives.length(); i++) {
                m_ev3->motor(i + 1)->setPower(drives[i]);
            }
        }
        break;

        case Meditation:
        case Concentration:
        {
            QString metaIndex = m_controlState == Meditation ? MEDITATION : CONCENTRATION;
            double metaIndexValue = m_controlState == Meditation ? m_meditation : m_concentration;
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
