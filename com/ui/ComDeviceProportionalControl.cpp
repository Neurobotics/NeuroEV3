#include "ComDeviceProportionalControl.h"

ComDeviceProportionalControl::ComDeviceProportionalControl(ComDevice *com, Settings *settings, bool meditation, QWidget *parent)
    : DeviceProportionalControl(settings, "com", meditation, parent)
{
    m_com = com;
}

void ComDeviceProportionalControl::innerSetValue()
{
    m_com->sendSpeed(m_value);
}
