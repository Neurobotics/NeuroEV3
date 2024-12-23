#ifndef COMDEVICEMANUALCONTROL_H
#define COMDEVICEMANUALCONTROL_H

#include <QWidget>
#include "com/ComDevice.h"

class ComDeviceManualControl : public QWidget
{
    Q_OBJECT
public:
    explicit ComDeviceManualControl(ComDevice *com, QWidget *parent = nullptr);

protected:
    ComDevice *m_com = nullptr;
};

#endif // COMDEVICEMANUALCONTROL_H
