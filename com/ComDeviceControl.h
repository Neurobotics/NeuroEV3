#ifndef COMDEVICECONTROL_H
#define COMDEVICECONTROL_H

#include <QWidget>
#include "com/ComDevice.h"

class ComDeviceControl : public QWidget
{
    Q_OBJECT
public:
    explicit ComDeviceControl(QWidget *parent = nullptr);

    void setComDevice(ComDevice *com);

signals:

protected:
    ComDevice *m_com = nullptr;
};

#endif // COMDEVICECONTROL_H
