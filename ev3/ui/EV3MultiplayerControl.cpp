#include "EV3MultiplayerControl.h"
#include "ev3/EV3_Motor.h"
#include "ui/MotorsCoeffWidget.h"

EV3MultiplayerControl::EV3MultiplayerControl(EV3 *ev3, Settings *settings, QWidget *parent) : DeviceMultiplayerControl(settings, parent)
{
    m_ev3 = ev3;

    auto multiplayerWidget = new MotorsCoeffWidget();
    for (int i = 1; i<=MAX_MOTORS; i++) {
        multiplayerWidget->setMotorCoeff(i, m_settings->getMetaIndexDriveCoeff(MULTIPLAYER, i));
        multiplayerWidget->setMotorEnabled(i, m_settings->getMetaIndexDriveEnabled(MULTIPLAYER, i));
    }
    connect(multiplayerWidget, &MotorsCoeffWidget::motorEnabledChanged, [=](int motorIndex, bool enabled) {
        m_settings->setMetaIndexDriveEnabled(MULTIPLAYER, motorIndex, enabled);
    });
    connect(multiplayerWidget, &MotorsCoeffWidget::motorCoeffChanged, [=](int motorIndex, double coeff) {
        m_settings->setMetaIndexDriveCoeff(MULTIPLAYER, motorIndex, coeff);
    });

    addWidget(multiplayerWidget, Qt::AlignTop | Qt::AlignHCenter);
}

void EV3MultiplayerControl::onSetValues()
{
    if (m_ev3 && isVisible()) {
        float val = m_useMeditation ? m_meditationMix : m_concentrationMix;
        for (int i = 1; i <= MAX_MOTORS; i++) {
            if (m_settings->getMetaIndexDriveEnabled(MULTIPLAYER, i)) {
                m_ev3->motor(i)->setPower(m_settings->getMetaIndexDriveCoeff(MULTIPLAYER, i) * val);
            } else {
                m_ev3->motor(i)->setPower(0);
            }
        }
    }
}
