#include "MotorsWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "IconLabel.h"
#include "Common.h"

MotorsWidget::MotorsWidget(QWidget *parent) : QWidget(parent)
{
    auto layoutMotors = new QVBoxLayout(this);
    layoutMotors->setSpacing(2);
    for (int i = 1; i<=4; i++) {
        auto motorPowerLabel = new QLabel("0");
        motorPowerLabel->setMinimumWidth(50);

        auto motorSocket = Common::Instance()->motorSocket(i);

        auto motorPower = new QSlider();
        motorPower->setRange(-100, 100);
        motorPower->setFixedWidth(208);
        motorPower->setTickInterval(100);
        motorPower->setTickPosition(QSlider::TickPosition::TicksAbove);
        motorPower->setValue(0);
        motorPower->setOrientation(Qt::Horizontal);
        connect(motorPower, QOverload<int>::of(&QSlider::valueChanged), this, [=](int val) {
            motorPowerLabel->setText(QString::number(val));
            emit motorValueChangeRequest(i, val);
            motorSocket->setActive(val != 0);
        }, Qt::QueuedConnection);

        auto btnZero = new QPushButton();
        btnZero->setIcon(QIcon(":/resources/zero.svg"));
        connect(btnZero, &QPushButton::clicked, [=]() {
            motorPower->setValue(0);
        });

        m_sliders << motorPower;

        auto motorLayout = new QHBoxLayout();
        motorLayout->setSpacing(4);



        // btnSocket->setText(QString::number(i));
        motorLayout->addWidget(motorSocket);
        motorLayout->addWidget(motorPower);
        motorLayout->addWidget(motorPowerLabel);
        motorLayout->addWidget(btnZero, 100, Qt::AlignLeft);

        layoutMotors->addLayout(motorLayout);
    }
    layoutMotors->addWidget(new QLabel(), 100);
}

void MotorsWidget::setMotorValue(int motorIndex, int value)
{
    motorIndex --;
    if (motorIndex >= 0 && motorIndex < m_sliders.length()) {
        m_sliders[motorIndex]->setValue(value);
    }
}
