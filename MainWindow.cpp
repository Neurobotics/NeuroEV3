#include "MainWindow.h"
#include "EV3.h"
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "texthelper.h"
#include <QSlider>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTabWidget>
#include "MotorsWidget.h"
#include <QCheckBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_settings = new Settings();

    auto labelConnected = new QLabel("?");
    auto func_connected = [=]() {
        labelConnected->setText(TextHelper::EnumToString<EV3::ConnectionState>(m_ev3->connectionState()));
    };

    m_ev3 = new EV3();
    connect(m_ev3, &EV3::connectionStateChanged, this, func_connected, Qt::QueuedConnection);

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
    for (int i = 1; i<=4; i++) {

        auto checkbox = new QCheckBox("State " + QString::number(i));
        checkbox->setChecked(m_settings->getMentalStateEnabled(i));
        connect(checkbox, &QCheckBox::toggled, [=](bool toggled) {
            m_settings->setMentalStateEnabled(i, toggled);
        });

        auto motors = new MotorsWidget();
        for (int j = 1; j<=4; j++) {
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
    headerLayout->addWidget(btnConnect);
    headerLayout->addWidget(labelConnected, 100);

    auto tabs = new QTabWidget();
    tabs->addTab(manualMotors, "Manual");
    tabs->addTab(bciStateMotors, "Mental states");

    auto grid = new QVBoxLayout(centralWidget);
    grid->addLayout(headerLayout);
    grid->addWidget(tabs);

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
    m_ev3->disconnect();
}
