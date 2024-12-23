#ifndef COMDEVICEPROPORTIONALCONTROL_H
#define COMDEVICEPROPORTIONALCONTROL_H

#include <QWidget>
#include "ComDevice.h"

class ComDeviceProportionalControl : public QWidget
{
    Q_OBJECT
public:
    explicit ComDeviceProportionalControl(ComDevice *com, QWidget *parent = nullptr);

signals:

protected:
    ComDevice *m_com = nullptr;
};

#endif // COMDEVICEPROPORTIONALCONTROL_H
