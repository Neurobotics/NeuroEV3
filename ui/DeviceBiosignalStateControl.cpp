#include "DeviceBiosignalStateControl.h"
#include "classes/Common.h"
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QTimer>
#include "ui/GroupBox.h"
#include "ui/LabelWithIcon.h"

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
        if (index >= m_lastStates.length()) {
            index = -1;
        }
        circle->setState(index >= 0 ? m_lastStates[index] : 0);
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
        int ii = wordLength - 1 - m_controlIndex;
        if (ii >= 0 && ii < m_controlStateCirles.length()) {
            m_controlStateCirles[ii]->setState(controlSymbol);
        }

        if (m_controlState == 0) {
            m_controlState = controlSymbol;
        } else {
            m_controlState += controlSymbol * pow(10, m_controlIndex);
        }

        m_controlIndex ++;
        if (m_controlIndex >= wordLength) {
            // qDebug() << "CTRL" << m_controlState;
            m_controlIndex = 0;
            for(int i = 0; i<m_controlStateCirles.length(); i++) {
                m_lastControlStateCirles[i]->setState(m_controlStateCirles[i]->state());
            }

            QTimer::singleShot(200, [=]() {
                for(int i = 0; i<m_controlStateCirles.length(); i++) {
                    m_controlStateCirles[i]->setState(0);
                }
            });

            onSetCurrentState(m_controlState);
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
        circle->setToolTip(tr("Biosignal state history"));
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
    });

    auto spinBSWordLength = new QSpinBox();
    spinBSWordLength->setRange(1, MAX_BIOSIGNAL_STATE_WORD_LENGTH);
    spinBSWordLength->setValue(m_settings->getBiosignalStatesWordLength());
    connect(spinBSWordLength, &QSpinBox::valueChanged, this, [=](int value) {
        m_settings->setBiosignalStatesWordLength(value);
        rebuildStateControls();
    });

    auto settingsLayout = new QHBoxLayout();
    settingsLayout->addWidget(new LabelWithIcon(QIcon(":/resources/help.svg"), tr("States"), tr("Amount of supported states\n(important if word length is greater than 1)")));
    settingsLayout->addWidget(spinBSMax);
    settingsLayout->addSpacing(20);
    settingsLayout->addWidget(new LabelWithIcon(QIcon(":/resources/help.svg"), tr("Repeats"), tr("The number of repetitions of the same state to count it as a state")));
    settingsLayout->addWidget(spinBSRepeats);
    settingsLayout->addSpacing(20);
    settingsLayout->addWidget(new LabelWithIcon(QIcon(":/resources/help.svg"), tr("Word length"), tr("The amount of symbols in metastate word")));
    settingsLayout->addWidget(spinBSWordLength);
    settingsLayout->addStretch(1);

    m_scroll = new NScrollViewer();
    m_scroll->setSpacing(0);

    auto controlStatesLayout = new QHBoxLayout();
    controlStatesLayout->setSpacing(0);
    for (int i = 0; i < MAX_BIOSIGNAL_STATE_WORD_LENGTH; i++) {
        auto square = new BiosignalStateCircle(0, false, QSize(32,32));
        square->setToolTip(tr("Last state"));
        square->setSquare(true);
        m_controlStateCirles << square;
        controlStatesLayout->addWidget(square);
    }
    controlStatesLayout->addSpacing(20);
    for (int i = 0; i < MAX_BIOSIGNAL_STATE_WORD_LENGTH; i++) {
        auto square = new BiosignalStateCircle(0, false, QSize(24,24));
        square->setFilled(true);
        square->setSquare(true);
        square->setToolTip(tr("Previous state"));
        m_lastControlStateCirles << square;
        controlStatesLayout->addWidget(square);
    }

    controlStatesLayout->addStretch(100);

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(new QLabel(QCoreApplication::translate("Generic", "Current state:")), 0);
    m_layout->addLayout(stateCirclesLayout, 0);
    m_layout->addSpacing(10);
    m_layout->addLayout(settingsLayout, 0);
    m_layout->addLayout(controlStatesLayout, 0);
    m_layout->addWidget(m_scroll, 100);

    rebuildStateControls();
}

void DeviceBiosignalStateControl::rebuildStateControls()
{
    m_scroll->clear();

    m_controlAccum.clear();
    m_controlState = 0;
    m_controlIndex = 0;

    for (int i = 0; i < m_lastControlStateCirles.length(); i++) {
        m_lastControlStateCirles[i]->setState(0);
        m_controlStateCirles[i]->setState(0);
    }

    int wordLength = m_settings->getBiosignalStatesWordLength();

    auto states = statesForWord(m_settings->getBiosignalStatesMax(), wordLength);
    foreach (auto state, states) {
        auto group = new GroupBox(QCoreApplication::translate("Generic", "State"), true);
        auto groupWidget = new QWidget();
        auto groupWidgetLayout = new QHBoxLayout(groupWidget);
        group->setChecked(m_settings->getMentalStateEnabled(state, m_stateEnabledPrefix));
        connect(group, &GroupBox::toggled, [=](bool toggled) {
            m_settings->setMentalStateEnabled(state, toggled, m_stateEnabledPrefix);
        });

        QList<int> stateDigits;
        int st = state;
        if (state > 100) {
            stateDigits << qFloor(st / 100);
            st = st % 100;
        }
        if (state > 10) {
            stateDigits << qFloor(st / 10);
            st = st % 10;
        }

        stateDigits << st;

        foreach (int digit, stateDigits) {
            auto b = new BiosignalStateCircle(digit, true, QSize(20, 20));
            b->setSquare(true);
            group->addWidget(b);
        }

        group->addHeaderWidget(new QLabel(" → "));

        group->addWidget(createStateWidget(state));

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
