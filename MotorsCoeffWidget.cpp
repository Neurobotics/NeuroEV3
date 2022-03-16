#include "MotorsCoeffWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "Common.h"

MotorsCoeffWidget::MotorsCoeffWidget(QWidget *parent) : QWidget(parent)
{
    auto layoutMotors = new QVBoxLayout(this);
    for (int i = 1; i<=4; i++) {
        auto motorSocket = Common::Instance()->motorSocket(i);

        auto checkbox = new QCheckBox();
        connect(checkbox, &QCheckBox::toggled, [=](bool toggled) {
           motorSocket->setActive(toggled);
           emit motorEnabledChanged(i, toggled);
        });
        m_checkboxes << checkbox;

        auto motorCoeff = new QDoubleSpinBox();
        motorCoeff->setRange(0, 2);
        motorCoeff->setSingleStep(0.1);
        motorCoeff->setValue(0);
        motorCoeff->setPrefix("k=");
        connect(motorCoeff, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val) {
            emit motorCoeffChanged(i, val);
        }, Qt::QueuedConnection);
        m_coeffs << motorCoeff;

        auto motorLayout = new QHBoxLayout();
        motorLayout->setSpacing(4);
        motorLayout->addWidget(checkbox);
        motorLayout->addWidget(motorSocket);
        motorLayout->addWidget(motorCoeff, 100, Qt::AlignLeft);

        layoutMotors->addLayout(motorLayout);
    }
    layoutMotors->addWidget(new QLabel(), 100);
}

void MotorsCoeffWidget::setMotorEnabled(int motorIndex, bool enabled)
{
    motorIndex --;
    if (motorIndex >= 0 && motorIndex < m_checkboxes.length()) {
        m_checkboxes[motorIndex]->setChecked(enabled);
    }
}

void MotorsCoeffWidget::setMotorCoeff(int motorIndex, double coeff)
{
    motorIndex --;
    if (motorIndex >= 0 && motorIndex < m_coeffs.length()) {
        m_coeffs[motorIndex]->setValue(coeff);
    }
}
