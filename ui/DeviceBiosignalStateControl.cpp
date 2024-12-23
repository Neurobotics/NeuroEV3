#include "DeviceBiosignalStateControl.h"
#include "classes/Common.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QCheckBox>
#include <QGroupBox>

DeviceBiosignalStateControl::DeviceBiosignalStateControl(Settings *settings, QString stateEnabledPrefix, QWidget *parent) : QWidget(parent)
{
    m_settings = settings;
    m_stateEnabledPrefix = stateEnabledPrefix;

    m_labelCurrentState = new QLabel("?");
}

void DeviceBiosignalStateControl::setCurrentState(int state)
{
    QString stateStr = QString::number(state);

    QString st = "<span style=\"font-weight: bold; font-size: 20px\">[" + stateStr + "]</span>";

    int n = m_lastStates.length();
    if (n > 0) {
        for (int i = m_lastStates.length() - 1; i >= 0; i--) {
            st += " [" + m_lastStates[i] + "]";
        }
    }

    m_lastStates << stateStr;
    while (m_lastStates.length() > 19) {
        m_lastStates.removeFirst();
    }

    m_labelCurrentState->setText(st);

    onSetCurrentState(state);
}

void DeviceBiosignalStateControl::init()
{
    auto statesWidget = new QWidget();
    auto statesLayout = new QVBoxLayout(statesWidget);

    for (int i = 1; i<=MAX_MENTAL_STATES; i++) {
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
    layout->addWidget(m_labelCurrentState);
    layout->addWidget(scroll);
}
