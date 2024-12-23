#ifndef COMDEVICESTATUSWIDGET_H
#define COMDEVICESTATUSWIDGET_H

#include <QWidget>
#include "ui/IconLabel.h"
#include "com/ComDevice.h"

class ComDeviceStatusWidget : public IconLabel
{
    Q_OBJECT
public:
    explicit ComDeviceStatusWidget(ComDevice *com, QWidget *parent = nullptr);

signals:
};

#endif // COMDEVICESTATUSWIDGET_H
