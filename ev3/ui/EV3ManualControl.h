#ifndef EV3MANUALCONTROL_H
#define EV3MANUALCONTROL_H

#include <QWidget>
#include "ev3/EV3.h"

class EV3ManualControl : public QWidget
{
    Q_OBJECT
public:
    explicit EV3ManualControl(EV3 *ev3, QWidget *parent = nullptr);

protected:
    EV3 *m_ev3 = nullptr;
};

#endif // EV3MANUALCONTROL_H
