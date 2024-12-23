#include "DeviceProportionalControl.h"
#include <QCoreApplication>
#include "ui/UICommon.h"
#include <QHBoxLayout>

DeviceProportionalControl::DeviceProportionalControl(bool meditation, bool useProportional, int threshold, QWidget *parent) : QWidget{parent}
{
    m_isUseMeditation = meditation;
    m_useProportional = useProportional;
    m_threshold = threshold;

    m_bar = meditation ? UICommon::meditationBar() : UICommon::concentrationBar();

    m_checkboxProportional = new QCheckBox(QCoreApplication::translate("Generic", "Proportional"));
    m_checkboxProportional->setChecked(m_useProportional);
    connect(m_checkboxProportional, &QCheckBox::toggled, [=](bool on) {
        m_useProportional = on;
        emit onUseProportional(on);
        innerSetProportional();
        m_spinThreshold->setVisible(!on);
    });

    m_spinThreshold = new QSpinBox();
    m_spinThreshold->setRange(1, 100);
    m_spinThreshold->setSuffix("%");
    m_spinThreshold->setPrefix(QCoreApplication::translate("Generic", "Threshold") + " ");
    m_spinThreshold->setValue(m_threshold);
    m_spinThreshold->setVisible(!m_useProportional);
    connect(m_spinThreshold, &QSpinBox::valueChanged, [=](int value) {
        m_threshold = qBound(0, value, 100);
        emit onSetThreshold(m_threshold);
        innerSetThreshold();
    });

    auto row = new QHBoxLayout();
    row->addWidget(m_checkboxProportional);
    row->addWidget(m_spinThreshold);
    row->addStretch(100);

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_bar);
    m_layout->addLayout(row);
}

void DeviceProportionalControl::setValue(int value)
{
    m_value = qBound(0, value, 100);
    if (m_bar) m_bar->setValue(m_value);
    innerSetValue();
}

void DeviceProportionalControl::innerSetValue()
{

}

void DeviceProportionalControl::innerSetProportional()
{

}

void DeviceProportionalControl::innerSetThreshold()
{

}
