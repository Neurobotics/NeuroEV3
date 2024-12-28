#include "EV3ManualControl.h"
#include "ev3/EV3_Motor.h"
#include "ev3/ui/MotorsWidget.h"

EV3ManualControl::EV3ManualControl(EV3 *ev3, QWidget *parent) : QWidget{parent}
{
    m_ev3 = ev3;

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

    auto lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(manualMotors);
    lay->addStretch(100);
}
