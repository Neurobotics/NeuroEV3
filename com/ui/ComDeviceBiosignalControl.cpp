#include "ComDeviceBiosignalControl.h"
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include "com/ui/ComCommandSelector.h"

ComDeviceBiosignalControl::ComDeviceBiosignalControl(ComDevice *com, Settings *settings, QWidget *parent)
    : DeviceBiosignalStateControl(settings, "com", parent)
{
    m_com = com;
    init();

    m_sequence = new SequencePlayer();
    m_sequence->setInterval(com->profile()->value("StatesSequenceTimeMs", 1000).toInt());
    connect(m_sequence, &SequencePlayer::intervalChanged, [=](int ms) {
        com->profile()->setValue("StatesSequenceTimeMs", ms);
    });

    m_sequence->enableSequence(com->profile()->value("StatesSequenceEnabled", false).toBool());
    connect(m_sequence, &SequencePlayer::sequenceEnableChanged, [=](bool on) {
        com->profile()->setValue("StatesSequenceEnabled", on);
    });

    m_sequence->setCanAcceptCommandVisible(true);

    m_layout->addWidget(m_sequence);
}

void ComDeviceBiosignalControl::onSetCurrentState(int state)
{
    if (m_com && isVisible()) {
        if (m_settings->getMentalStateEnabled(state, m_stateEnabledPrefix)) {
            auto actionName = "State" +QString::number(state);
            if (m_sequence && m_sequence->sequenceEnabled()) {
                auto action = m_com->action(actionName);
                if (!action.isEmpty()) {
                    auto command = ComDevice::CommandByName(action);
                    m_sequence->addCommand(action, command.symbol);
                }
            } else {
                m_com->performAction(actionName);
            }
        } else {
            m_com->stop();
        }
    }
}

QWidget *ComDeviceBiosignalControl::createStateWidget(int state)
{    
    return new ComCommandSelector(m_com, "State" +QString::number(state));
}
