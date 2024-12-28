#ifndef EV3BIOSIGNALSTATECONTROL_H
#define EV3BIOSIGNALSTATECONTROL_H

#include "ev3/EV3.h"
#include "classes/Settings.h"
#include "ui/DeviceBiosignalStateControl.h"

class EV3BiosignalStateControl : public DeviceBiosignalStateControl
{
    Q_OBJECT
public:
    explicit EV3BiosignalStateControl(EV3 *ev3, Settings *settings, QWidget *parent = nullptr);

protected:
    EV3* m_ev3 = nullptr;

    QWidget *createStateWidget(int state) override;
    void onSetCurrentState(int state) override;
};

#endif // EV3BIOSIGNALSTATECONTROL_H
