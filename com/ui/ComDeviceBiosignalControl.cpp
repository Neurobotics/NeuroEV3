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
