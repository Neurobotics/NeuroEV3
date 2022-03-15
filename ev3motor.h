#ifndef EV3MOTOR_H
#define EV3MOTOR_H

#include <QObject>
#include "ev3.h"
#include "ev3command.h"

class ev3motor : public QObject
{
    Q_OBJECT
public:
    ev3motor(Ev3 *ev3, int port, QObject *parent = nullptr);

    void setPower(int power);
    void start();
    void stop();
    void setPolarity(int p);
    void setSpeedControlEnabled(bool enable);

signals:
    void powerChanged();

protected:
    int m_port = 0;
    bool m_polarity = true;
    int m_power = 100;
    Ev3 *m_ev3 = nullptr;
    bool m_enableSpeedControl = false;
};

#endif // EV3MOTOR_H
