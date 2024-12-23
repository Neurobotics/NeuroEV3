#ifndef COMDEVICEBIOSIGNALCONTROL_H
#define COMDEVICEBIOSIGNALCONTROL_H

#include "ui/DeviceBiosignalStateControl.h"
#include "com/ComDevice.h"
#include "classes/Settings.h"

class ComDeviceBiosignalControl : public DeviceBiosignalStateControl
{
    Q_OBJECT
public:
    explicit ComDeviceBiosignalControl(ComDevice *com, Settings *settings, QWidget *parent = nullptr);

protected:
    ComDevice *m_com = nullptr;

    void onSetCurrentState(int state) override;
    QWidget *createStateWidget(int state, Qt::Alignment align = Qt::Alignment()) override;
};

#endif // COMDEVICEBIOSIGNALCONTROL_H
