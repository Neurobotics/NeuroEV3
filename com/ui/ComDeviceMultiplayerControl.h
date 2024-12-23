#ifndef COMMULTIPLAYERCONTROL_H
#define COMMULTIPLAYERCONTROL_H

#include "ui/DeviceMultiplayerControl.h"
#include "com/ComDevice.h"

class ComDeviceMultiplayerControl : public DeviceMultiplayerControl
{
    Q_OBJECT
public:
    explicit ComDeviceMultiplayerControl(ComDevice *com, Settings *settings, QWidget *parent = nullptr);

protected:
    ComDevice *m_com = nullptr;
    QString m_prefix = "multiK";

    void onSetValues() override;
};

#endif // COMMULTIPLAYERCONTROL_H
