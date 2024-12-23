#ifndef COMDEVICEPROPORTIONALCONTROL_H
#define COMDEVICEPROPORTIONALCONTROL_H

#include "ui/DeviceProportionalControl.h"
#include "com/ComDevice.h"
#include "classes/Settings.h"

class ComDeviceProportionalControl : public DeviceProportionalControl
{
    Q_OBJECT
public:
    explicit ComDeviceProportionalControl(ComDevice *com, Settings *settings, bool meditation, QWidget *parent = nullptr);

protected:
    ComDevice *m_com = nullptr;
    void innerSetValue() override;
    QString m_prefix = "k";
};

#endif // COMDEVICEPROPORTIONALCONTROL_H
