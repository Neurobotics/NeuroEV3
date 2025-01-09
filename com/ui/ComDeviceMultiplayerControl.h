#ifndef COMMULTIPLAYERCONTROL_H
#define COMMULTIPLAYERCONTROL_H

#include "ui/DeviceMultiplayerControl.h"
#include "com/ComDevice.h"
#include <QLabel>

class ComDeviceMultiplayerControl : public DeviceMultiplayerControl
{
    Q_OBJECT
public:
    explicit ComDeviceMultiplayerControl(ComDevice *com, Settings *settings, QWidget *parent = nullptr);

protected:
    ComDevice *m_com = nullptr;
    QString m_prefix = "multiK";

    void onSetValues() override;
    void onSetControlMode() override;

    QLabel *m_labelControlMode = nullptr;
};

#endif // COMMULTIPLAYERCONTROL_H
