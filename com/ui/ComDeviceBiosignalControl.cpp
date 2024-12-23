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

}

QWidget *ComDeviceBiosignalControl::createStateWidget(int state, Qt::Alignment align)
{    
    return new ComCommandSelector(m_com, "State" +QString::number(state));
}
