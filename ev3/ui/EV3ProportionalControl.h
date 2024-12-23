#ifndef EV3PROPORTIONALCONTROL_H
#define EV3PROPORTIONALCONTROL_H

#include "ui/DeviceProportionalControl.h"
#include "ev3/EV3.h"
#include "classes/Settings.h"

class EV3ProportionalControl : public DeviceProportionalControl
{
    Q_OBJECT
public:
    explicit EV3ProportionalControl(EV3 *ev3, Settings *settings, bool meditation, bool useProportional = true, int threshold = 50, QWidget *parent = nullptr);

protected:
    EV3 *m_ev3 = nullptr;
    Settings *m_settings = nullptr;
    QString m_metaIndex = "";

    void innerSetValue() override;
};

#endif // EV3PROPORTIONALCONTROL_H
