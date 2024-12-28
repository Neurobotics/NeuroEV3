#include "MotorsWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "ui/IconLabel.h"
#include "classes/Common.h"
#include <QCoreApplication>
#include "ev3/EV3.h"

MotorsWidget::MotorsWidget(QWidget *parent) : BaseMotorsWidget(parent)
{
    m_layout->setSpacing(2);
    for (int i = 1; i <= EV3_MAX_MOTORS; i++) {
        auto motorPowerLabel = new QLabel("0");
        motorPowerLabel->setMinimumWidth(50);

        auto motorSocket = Common::Instance()->motorSocket(i);

        auto motorPower = new ImprovedSlider();
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
        btnZero->setCursor(Qt::PointingHandCursor);
        btnZero->setToolTip(QCoreApplication::translate("Generic", "Set to zero"));
        btnZero->setFixedSize(24, 24);
        connect(btnZero, &QPushButton::clicked, [=]() {
            motorPower->setValue(0);
        });

        m_sliders << motorPower;

        auto motorLayout = new QHBoxLayout();
        motorLayout->setSpacing(4);

        motorLayout->addWidget(motorSocket);
        motorLayout->addWidget(motorPower);
        motorLayout->addWidget(motorPowerLabel);
        motorLayout->addWidget(btnZero, 100, Qt::AlignLeft);

        m_layout->addLayout(motorLayout);
    }
}

void MotorsWidget::setMotorValue(int motorIndex, int value)
{
    motorIndex --;
    if (motorIndex >= 0 && motorIndex < m_sliders.length()) {
        m_sliders[motorIndex]->setValue(value);
    }
}
