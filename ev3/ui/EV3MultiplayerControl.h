#ifndef EV3MULTIPLAYERCONTROL_H
#define EV3MULTIPLAYERCONTROL_H

#include "ui/DeviceMultiplayerControl.h"
#include "ev3/EV3.h"

class EV3MultiplayerControl : public DeviceMultiplayerControl
{
    Q_OBJECT
public:
    explicit EV3MultiplayerControl(EV3 *ev3, Settings *settings, QWidget *parent = nullptr);

protected:
    EV3* m_ev3 = nullptr;
    void onSetValues() override;
};

#endif // EV3MULTIPLAYERCONTROL_H
