#include "DeviceBiosignalStateControl.h"
#include "classes/Common.h"
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QTimer>

DeviceBiosignalStateControl::DeviceBiosignalStateControl(Settings *settings, QString stateEnabledPrefix, QWidget *parent) : QWidget(parent)
{
    m_settings = settings;
    m_stateEnabledPrefix = stateEnabledPrefix;
}

void DeviceBiosignalStateControl::setCurrentState(int state)
{
    m_lastStates << state;
    while (m_lastStates.length() - 1 > m_maxDisplayedStates) {
        m_lastStates.removeFirst();
    }

    int n = m_lastStates.length();

    for (int i = 0, m = m_stateCircles.length(); i < m; i++) {
        auto circle = m_stateCircles[i];
        int index = n - 1 - i;
        circle->setState(index > 0 ? m_lastStates[index] : 0);
    }

    int amountToControl = m_settings->getBiosignalStatesAmountToControl();
    // int maxStates = m_settings->getBiosignalStatesMax();
    int wordLength = m_settings->getBiosignalStatesWordLength();

    if (amountToControl == 1 && wordLength == 1) {
        onSetCurrentState(state);
        return;
    }

    int controlSymbol = 0;
    bool controlFetched = false;
    if (amountToControl == 1) {
        controlSymbol = state;
        controlFetched = true;
    } else {
        if (m_controlAccum.length() == 0) {
            m_controlAccum << state;
        } else {
            if (m_controlAccum.last() == state) {
                m_controlAccum << state;
            } else {
                m_controlAccum.clear();
                m_controlAccum << state;
            }
        }

        controlFetched = m_controlAccum.length() == amountToControl;

        if (controlFetched) {
            controlSymbol = m_controlAccum.last();
            m_controlAccum.clear();
        }
    }

    if (controlFetched) {
        // qDebug() << "NEW SYMBOL" << controlSymbol << m_controlIndex << (wordLength - 1 - m_controlIndex) << m_controlStateCirles.length();
        m_controlStateCirles[wordLength - 1 - m_controlIndex]->setState(controlSymbol);

        if (m_controlState == 0) {
            m_controlState = controlSymbol;
        } else {
            m_controlState += controlSymbol * pow(10, m_controlIndex);
        }


        m_controlIndex ++;
        if (m_controlIndex >= wordLength) {
            m_controlIndex = 0;
            for(int i = 0; i<m_controlStateCirles.length(); i++) {
                m_lastControlStateCirles[i]->setState(m_controlStateCirles[i]->state());
            }

            QTimer::singleShot(200, [=]() {
                for(int i = 0; i<m_controlStateCirles.length(); i++) {
                    m_controlStateCirles[i]->setState(0);
                }
            });

            onSetCurrentState(state);
            m_controlAccum.clear();
            m_controlState = 0;
        }
    }
}

void DeviceBiosignalStateControl::init()
{
    auto stateCirclesLayout = new QHBoxLayout();
    stateCirclesLayout->setSpacing(0);

    for (int i = 0; i < m_maxDisplayedStates; i++) {
        auto circle = new BiosignalStateCircle(0, false, i == 0 ? QSize(40,40) : QSize(24, 24));
        stateCirclesLayout->addWidget(circle);
        m_stateCircles << circle;
    }
    stateCirclesLayout->addStretch(100);

    auto spinBSMax = new QSpinBox();
    spinBSMax->setRange(2, MAX_MENTAL_STATES);
    spinBSMax->setValue(m_settings->getBiosignalStatesMax());
    connect(spinBSMax, &QSpinBox::valueChanged, this, [=](int value) {
        m_settings->setBiosignalStatesMax(value);
        rebuildStateControls();
    });

    auto spinBSRepeats = new QSpinBox();
    spinBSRepeats->setRange(1, 10);
    spinBSRepeats->setValue(m_settings->getBiosignalStatesAmountToControl());
    connect(spinBSRepeats, &QSpinBox::valueChanged, this, [=](int value) {
        m_settings->setBiosignalStatesAmountToControl(value);
        // rebuildStateControls();
    });

    auto spinBSWordLength = new QSpinBox();
    spinBSWordLength->setRange(1, MAX_BIOSIGNAL_STATE_WORD_LENGTH);
    spinBSWordLength->setValue(m_settings->getBiosignalStatesWordLength());
    connect(spinBSWordLength, &QSpinBox::valueChanged, this, [=](int value) {
        m_settings->setBiosignalStatesWordLength(value);
        rebuildStateControls();
    });

    auto settingsLayout = new QHBoxLayout();
    settingsLayout->addWidget(new QLabel("Состояний"));
    settingsLayout->addWidget(spinBSMax);
    settingsLayout->addSpacing(10);
    settingsLayout->addWidget(new QLabel("Повторы"));
    settingsLayout->addWidget(spinBSRepeats);
    settingsLayout->addSpacing(10);
    settingsLayout->addWidget(new QLabel("Длина слова"));
    settingsLayout->addWidget(spinBSWordLength);
    settingsLayout->addStretch(1);

    m_scroll = new NScrollViewer();
    m_scroll->setSpacing(0);

    auto controlStatesLayout = new QHBoxLayout();
    controlStatesLayout->setSpacing(0);
    for (int i = 0; i < MAX_BIOSIGNAL_STATE_WORD_LENGTH; i++) {
        auto square = new BiosignalStateCircle(0, false, QSize(32,32));
        square->setSquare(true);
        m_controlStateCirles << square;
        controlStatesLayout->addWidget(square);
    }
    controlStatesLayout->addSpacing(20);
    for (int i = 0; i < MAX_BIOSIGNAL_STATE_WORD_LENGTH; i++) {
        auto square = new BiosignalStateCircle(0, false, QSize(24,24));
        square->setSquare(true);
        m_lastControlStateCirles << square;
        controlStatesLayout->addWidget(square);
    }

    controlStatesLayout->addStretch(100);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(QCoreApplication::translate("Generic", "Current state:")), 0);
    layout->addLayout(stateCirclesLayout, 0);
    layout->addSpacing(10);
    layout->addLayout(settingsLayout, 0);
    layout->addLayout(controlStatesLayout, 0);
    layout->addWidget(m_scroll, 100);

    rebuildStateControls();
}

void DeviceBiosignalStateControl::rebuildStateControls()
{
    m_scroll->clear();

    int wordLength = m_settings->getBiosignalStatesWordLength();

    auto states = statesForWord(m_settings->getBiosignalStatesMax(), wordLength);
    foreach (auto state, states) {
        auto groupWidget = new QWidget();
        auto groupWidgetLayout = new QHBoxLayout(groupWidget);
        auto group = new QGroupBox(QCoreApplication::translate("Generic", "State") + " " + QString::number(state));
        group->setCheckable(true);
        group->setChecked(m_settings->getMentalStateEnabled(state, m_stateEnabledPrefix));
        connect(group, &QGroupBox::toggled, [=](bool toggled) {
            m_settings->setMentalStateEnabled(state, toggled, m_stateEnabledPrefix);
        });


        auto lay = new QVBoxLayout(group);
        auto w = createStateWidget(state);
        if (w) {
            lay->addWidget(w);
        }

        groupWidgetLayout->addWidget(group, 0, Qt::AlignLeft);

        m_scroll->addWidget(groupWidget);
    }

    for (int i = 0; i<m_controlStateCirles.length(); i++) {
        m_controlStateCirles[i]->setVisible(i < wordLength);
        m_lastControlStateCirles[i]->setVisible(i < wordLength);
    }
}

QList<int> DeviceBiosignalStateControl::statesForWord(int maxStates, int wordLength)
{
    QList<int> states;
    if (wordLength == 1) {
        for (int i = 1; i<=maxStates; i++) {
            states << i;
        }
    } else if (wordLength == 2) {
        for (int j = 1; j<=maxStates; j++) {
            for (int i = 1; i<=maxStates; i++) {
                states << j * 10  +  i;
            }
        }
    } else if (wordLength == 3) {
        for (int k = 1; k<=maxStates; k++) {
            for (int j = 1; j<=maxStates; j++) {
                for (int i = 1; i<=maxStates; i++) {
                    states << k * 100 + j * 10  +  i;
                }
            }
        }
    }
    return states;
}
