#include "ComDeviceProportionalControl.h"
#include <QDoubleSpinBox>
#include <QCoreApplication>
#include <QLabel>

ComDeviceProportionalControl::ComDeviceProportionalControl(ComDevice *com, Settings *settings, bool meditation, QWidget *parent)
    : DeviceProportionalControl(settings, "com", meditation, parent)
{
    m_com = com;

    m_prefix = meditation ? "meditationK" : "concentrationK";

    QString control = meditation ? QCoreApplication::translate("Generic", "Meditation") : QCoreApplication::translate("Generic", "Concentration");

    auto spinCoeff = new QDoubleSpinBox();
    spinCoeff->setRange(-2, 2);
    spinCoeff->setSingleStep(0.1);
    spinCoeff->setValue(m_com->profile()->value(m_prefix, 1).toDouble());
    spinCoeff->setPrefix("k=");
    connect(spinCoeff, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double val) {
        m_com->profile()->setValue(m_prefix, val);
    }, Qt::QueuedConnection);

    m_layout->addWidget(new QLabel(""));
    m_layout->addWidget(new QLabel(QCoreApplication::translate("Generic", "Multiplier")));
    m_layout->addWidget(spinCoeff, 0, Qt::AlignLeft);

    m_layout->addWidget(new QLabel(""));
    m_layout->addWidget(new QLabel("<span style=\"color: #159\">" + QCoreApplication::translate("Generic", "Speed") + "</span> = k * " + control));
    m_layout->addStretch(1);
}

void ComDeviceProportionalControl::innerSetValue()
{
    if (m_com && isVisible()) {
        m_com->sendSpeed(m_value * m_com->profile()->value(m_prefix, 1).toDouble());
    }
}
