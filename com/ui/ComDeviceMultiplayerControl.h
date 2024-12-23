#ifndef COMMULTIPLAYERCONTROL_H
#define COMMULTIPLAYERCONTROL_H

#include "ui/DeviceMultiplayerControl.h"
#include "com/ComDevice.h"

class ComDeviceMultiplayerControl : public DeviceMultiplayerControl
{
    Q_OBJECT
public:
    explicit ComDeviceMultiplayerControl(ComDevice *com, Settings *settings, QWidget *parent = nullptr);

};

#endif // COMMULTIPLAYERCONTROL_H
