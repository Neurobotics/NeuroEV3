#include "EV3BiosignalStateControl.h"
#include <QCheckBox>
#include "ev3/EV3_Motor.h"
#include "ui/MotorsWidget.h"

EV3BiosignalStateControl::EV3BiosignalStateControl(EV3 *ev3, Settings *settings, QWidget *parent)
    : DeviceBiosignalStateControl(settings, "", parent)
{
    m_ev3 = ev3;
    init();
}

QWidget *EV3BiosignalStateControl::createStateWidget(int state, Qt::Alignment align)
{
    auto motors = new MotorsWidget();
    motors->layout()->setContentsMargins(0,0,0,0);
    for (int j = 1; j<=MAX_MOTORS; j++) {
        motors->setMotorValue(j, m_settings->getMentalStateDrive(state, j));
    }

    connect(motors, &MotorsWidget::motorValueChangeRequest, [=](int motorIndex, int value) {
        m_settings->setMentalStateDrive(state, motorIndex, value);
    });

    return motors;
}

void EV3BiosignalStateControl::onSetCurrentState(int state)
{
    if (!m_ev3 || !isVisible()) return;

    if (m_settings->getMentalStateEnabled(state)) {
        auto drives = m_settings->getMentalStateDrives(state);
        for (int i = 0; i < drives.length(); i++) {
            m_ev3->motor(i + 1)->setPower(drives[i]);
        }
    } else {
        m_ev3->stopMotors();
    }
}
