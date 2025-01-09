#include "ComDeviceMultiplayerControl.h"
#include <QDoubleSpinBox>
#include <QCoreApplication>

ComDeviceMultiplayerControl::ComDeviceMultiplayerControl(ComDevice *com, Settings *settings, QWidget *parent)
    : DeviceMultiplayerControl(settings, parent)
{
    m_com = com;

    auto spinCoeff = new QDoubleSpinBox();
    spinCoeff->setRange(-2, 2);
    spinCoeff->setSingleStep(0.1);
    spinCoeff->setValue(m_com->profile()->value(m_prefix, 1).toDouble());
    spinCoeff->setPrefix("k=");
    connect(spinCoeff, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val) {
        m_com->profile()->setValue(m_prefix, val);
    }, Qt::QueuedConnection);

    m_layout->addWidget(new QLabel(""));
    m_layout->addWidget(new QLabel(QCoreApplication::translate("Generic", "Multiplier")), 0, Qt::AlignCenter);
    m_layout->addWidget(spinCoeff, 0, Qt::AlignCenter);

    m_labelControlMode = new QLabel();

    m_layout->addWidget(new QLabel(""));
    m_layout->addWidget(m_labelControlMode, 0, Qt::AlignCenter);
    m_layout->addStretch(1);

    onSetControlMode();
}

void ComDeviceMultiplayerControl::onSetValues()
{
    if (m_com && isVisible()) {
        float val = m_useMeditation ? m_meditationMix : m_concentrationMix;
        m_com->sendSpeed(val * m_com->profile()->value(m_prefix, 1).toDouble());
    }
}

void ComDeviceMultiplayerControl::onSetControlMode()
{
    if (!m_labelControlMode) return;

    QString control = m_useMeditation ? QCoreApplication::translate("Generic", "Meditation difference") : QCoreApplication::translate("Generic", "Concentration difference");
    m_labelControlMode->setText("<span style=\"color: #159\">" + QCoreApplication::translate("Generic", "Speed") + "</span> = k * (" + control + ")");
}
