#ifndef COMDEVICEMENTALCONTROL_H
#define COMDEVICEMENTALCONTROL_H

#include <QWidget>
#include "ComDevice.h"

class ComDeviceMentalControl : public QWidget
{
    Q_OBJECT
public:
    explicit ComDeviceMentalControl(ComDevice *com, QWidget *parent = nullptr);

signals:
};

#endif // COMDEVICEMENTALCONTROL_H
