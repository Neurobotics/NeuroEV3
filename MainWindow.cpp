#include "MainWindow.h"
#include "ev3.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "texthelper.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    auto labelConnected = new QLabel("?");
    auto func_connected = [=]() {
        labelConnected->setText(TextHelper::EnumToString<Ev3::ConnectionState>(m_ev3->connectionState()));
    };

    auto btnConnect = new QPushButton("Подключение");
    btnConnect->setCheckable(true);
    connect(btnConnect, &QPushButton::toggled, this, [=](bool toggled) {
        if (toggled) {
            m_ev3->searchAndConnect();
        } else {
            m_ev3->disconnect();
        }
    });

    auto layoutMotors = new QVBoxLayout();
    layoutMotors->addWidget(new QLabel("Моторы"));

    m_ev3 = new Ev3();
    connect(m_ev3, &Ev3::connectionStateChanged, this, func_connected, Qt::QueuedConnection);
    for (int i = 1; i<=4; i++) {
        m_motors << new ev3motor(m_ev3, i);
        auto motorLayout = new QHBoxLayout();
        motorLayout->addWidget(new QLabel(QString::number(i)));
        layoutMotors->addLayout(motorLayout);
    }

    auto centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    auto headerLayout = new QHBoxLayout();
    headerLayout->addWidget(btnConnect);
    headerLayout->addWidget(labelConnected, 100);

    auto grid = new QVBoxLayout(centralWidget);

    grid->addLayout(headerLayout);
    grid->addLayout(layoutMotors, 100);
}

MainWindow::~MainWindow()
{

}

