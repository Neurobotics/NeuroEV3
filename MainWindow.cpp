#include "MainWindow.h"
#include "EV3.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "texthelper.h"
#include <QSlider>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    auto labelConnected = new QLabel("?");
    auto func_connected = [=]() {
        labelConnected->setText(TextHelper::EnumToString<EV3::ConnectionState>(m_ev3->connectionState()));
    };

    auto btnConnect = new QPushButton("Подключение");
    btnConnect->setCheckable(true);
    connect(btnConnect, &QPushButton::toggled, this, [=](bool toggled) {
        if (toggled) {
            m_ev3->searchAndConnect();
        } else {
            foreach (auto motor, m_motors) {
                motor->stop();
            }
            m_ev3->disconnect();
        }
    });

    auto layoutMotors = new QVBoxLayout();
    layoutMotors->addWidget(new QLabel("Моторы"));

    m_ev3 = new EV3();
    connect(m_ev3, &EV3::connectionStateChanged, this, func_connected, Qt::QueuedConnection);
    for (int i = 1; i<=4; i++) {
        auto motor = new EV3_Motor(m_ev3, i);
        m_motors << motor;

        auto motorPowerLabel = new QLabel("0");
        motorPowerLabel->setMinimumWidth(50);

        auto motorPower = new QSlider();
        motorPower->setRange(-100, 100);
        motorPower->setValue(0);
        motorPower->setOrientation(Qt::Horizontal);
        connect(motorPower, QOverload<int>::of(&QSlider::valueChanged), [=](int val) {
            motor->setPower(val);
            motorPowerLabel->setText(QString::number(val));
        });

        auto btnOnOff = new QPushButton("Off");
        btnOnOff->setCheckable(true);
        connect(btnOnOff, &QPushButton::toggled, [=](bool toggled) {
            motor->start(toggled);
            btnOnOff->setText(toggled ? "On" : "Off");
        });

        auto motorLayout = new QHBoxLayout();
        motorLayout->addWidget(new QLabel(QString::number(i)));
        motorLayout->addWidget(btnOnOff);
        motorLayout->addWidget(motorPower, 100);
        motorLayout->addWidget(motorPowerLabel);

        layoutMotors->addLayout(motorLayout);
    }
    layoutMotors->addWidget(new QLabel(), 100);

    auto centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    auto headerLayout = new QHBoxLayout();
    headerLayout->addWidget(btnConnect);
    headerLayout->addWidget(labelConnected, 100);

    auto grid = new QVBoxLayout(centralWidget);

    grid->addLayout(headerLayout);
    grid->addLayout(layoutMotors, 100);

    auto labelBCIConnected = new QLabel("?");
    headerLayout->addWidget(labelBCIConnected);

    socket = new QWebSocket();
    connect(socket, &QWebSocket::connected, [=]()
    {
        m_state = EV3::ConnectionState::Connected;
        labelBCIConnected->setText("BCI connected");
    });
    connect(socket, &QWebSocket::disconnected, [=]()
    {
        m_state = EV3::ConnectionState::Disconnected;
        labelBCIConnected->setText("BCI disconnected");
    });
    connect(socket, &QWebSocket::textMessageReceived, this, [=](QString txt) {
        QJsonDocument json = QJsonDocument::fromJson(txt.toUtf8());
        QJsonObject resp = json.object();
        QString cmd = resp["command"].toString();
        //bool result = resp["result"].toBool();

        auto med = resp["meditation"];
        if (!med.isNull()) {
            meditation = med.toDouble();


        }

        auto con = resp["concentration"];
        if (!con.isNull()) {
            concentration = con.toDouble();
        }

        auto st = resp["mental_state"];
        if (!st.isNull()) {
            m_mentalState = st.toInt();

            if (m_mentalState == 2) {
                m_motors[1]->setPower(50);
                m_motors[2]->setPower(50);
            }
            else if (m_mentalState == 3) {
                m_motors[1]->setPower(50);
                m_motors[2]->setPower(-50);
            } else {
                m_motors[1]->setPower(0);
                m_motors[2]->setPower(0);
            }
        }
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
}

MainWindow::~MainWindow()
{
    foreach (auto motor, m_motors) {
        motor->stop();
    }
}

