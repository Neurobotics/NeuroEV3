#include "EV3ProportionalControl.h"
#include "ev3/EV3_Motor.h"
#include "ui/MotorsCoeffWidget.h"

EV3ProportionalControl::EV3ProportionalControl(EV3 *ev3, Settings *settings, bool meditation, bool useProportional, int threshold, QWidget *parent)
    : DeviceProportionalControl(meditation, useProportional, threshold, parent)
{
    m_ev3 = ev3;
    m_settings = settings;
    m_metaIndex = m_isUseMeditation ? MEDITATION : CONCENTRATION;

    auto bciMetaIndexMotors = new MotorsCoeffWidget();
    for (int i = 1; i<=MAX_MOTORS; i++) {
        bciMetaIndexMotors->setMotorCoeff(i, settings->getMetaIndexDriveCoeff(m_metaIndex, i));
        bciMetaIndexMotors->setMotorEnabled(i, settings->getMetaIndexDriveEnabled(m_metaIndex, i));
    }
    connect(bciMetaIndexMotors, &MotorsCoeffWidget::motorEnabledChanged, [=](int motorIndex, bool enabled) {
        settings->setMetaIndexDriveEnabled(m_metaIndex, motorIndex, enabled);
    });
    connect(bciMetaIndexMotors, &MotorsCoeffWidget::motorCoeffChanged, [=](int motorIndex, double coeff) {
        settings->setMetaIndexDriveCoeff(m_metaIndex, motorIndex, coeff);
    });

    m_layout->addWidget(bciMetaIndexMotors);
}

void EV3ProportionalControl::innerSetValue()
{
    if (m_ev3 && isVisible()) {
        for (int i = 1; i <= MAX_MOTORS; i++) {
            int power = m_settings->getMetaIndexDriveEnabled(m_metaIndex, i) ? m_settings->getMetaIndexDriveCoeff(m_metaIndex, i) * m_value : 0;
            m_ev3->motor(i)->setPower(power);
        }
    }
}
