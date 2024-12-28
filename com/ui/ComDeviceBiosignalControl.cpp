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
}

void ComDeviceBiosignalControl::onSetCurrentState(int state)
{
    if (m_com && isVisible())
    {
        if (m_settings->getMentalStateEnabled(state, m_stateEnabledPrefix)) {
            m_com->performAction("State" +QString::number(state));
        } else {
            m_com->stop();
        }
    }
}

QWidget *ComDeviceBiosignalControl::createStateWidget(int state)
{    
    return new ComCommandSelector(m_com, "State" +QString::number(state));
}
