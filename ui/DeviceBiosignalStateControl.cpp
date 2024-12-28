#include "DeviceBiosignalStateControl.h"
#include "classes/Common.h"
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>

DeviceBiosignalStateControl::DeviceBiosignalStateControl(Settings *settings, QString stateEnabledPrefix, QWidget *parent) : QWidget(parent)
{
    m_settings = settings;
    m_stateEnabledPrefix = stateEnabledPrefix;
}

void DeviceBiosignalStateControl::setCurrentState(int state)
{
    m_lastStates << state;
    while (m_lastStates.length() > m_maxDisplayedStates) {
        m_lastStates.removeFirst();
    }

    int n = m_lastStates.length();

    for (int i = 0, m = m_stateCircles.length(); i < m; i++) {
        auto circle = m_stateCircles[i];
        int index = n - 1 - i;
        circle->setState(index > 0 ? m_lastStates[index] : 0);
    }

    onSetCurrentState(state);
}

void DeviceBiosignalStateControl::init()
{
    auto statesWidget = new QWidget();
    auto stateCirclesLayout = new QHBoxLayout();
    stateCirclesLayout->setSpacing(0);

    for (int i = 0; i < m_maxDisplayedStates; i++) {
        auto circle = new BiosignalStateCircle(0, false, i == 0 ? QSize(40,40) : QSize(24, 24));
        stateCirclesLayout->addWidget(circle);
        m_stateCircles << circle;
    }
    stateCirclesLayout->addStretch(100);

    auto statesLayout = new QVBoxLayout(statesWidget);

    for (int i = 1; i <= MAX_MENTAL_STATES; i++) {
        auto group = new QGroupBox(QCoreApplication::translate("Generic", "State") + " " + QString::number(i));
        group->setCheckable(true);
        group->setChecked(m_settings->getMentalStateEnabled(i, m_stateEnabledPrefix));
        connect(group, &QGroupBox::toggled, [=](bool toggled) {
            m_settings->setMentalStateEnabled(i, toggled, m_stateEnabledPrefix);
        });

        auto lay = new QVBoxLayout(group);

        auto w = createStateWidget(i);
        if (w) {
            lay->addWidget(w);
        }

        statesLayout->addWidget(group);
    }

    auto scroll = new QScrollArea();
    scroll->setWidget(statesWidget);
    scroll->setStyleSheet("QScrollArea { background: transparent; border: 0; } QScrollArea > QWidget > QWidget { background: transparent; } ");

    auto layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(QCoreApplication::translate("Generic", "Current state:")));
    layout->addLayout(stateCirclesLayout);
    layout->addWidget(scroll);
}
