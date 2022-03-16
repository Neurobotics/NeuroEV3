#include "MotorsWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

MotorsWidget::MotorsWidget(QWidget *parent) : QWidget(parent)
{
    auto layoutMotors = new QVBoxLayout(this);
    layoutMotors->setSpacing(2);
    for (int i = 1; i<=4; i++) {
        auto motorPowerLabel = new QLabel("0");
        motorPowerLabel->setMinimumWidth(50);

        auto motorPower = new QSlider();
        motorPower->setRange(-100, 100);
        motorPower->setValue(0);
        motorPower->setOrientation(Qt::Horizontal);
        connect(motorPower, QOverload<int>::of(&QSlider::valueChanged), this, [=](int val) {
            motorPowerLabel->setText(QString::number(val));
            emit motorValueChangeRequest(i, val);
        }, Qt::QueuedConnection);

        m_sliders << motorPower;

        auto motorLayout = new QHBoxLayout();
        motorLayout->addWidget(new QLabel("M" + QString::number(i)));
        motorLayout->addWidget(motorPower, 100);
        motorLayout->addWidget(motorPowerLabel);

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
