#ifndef EV3_MOTOR_H
#define EV3_MOTOR_H

#include <QObject>
#include "EV3.h"
#include "EV3_Command.h"

class EV3_Motor : public QObject
{
    Q_OBJECT
public:
    EV3_Motor(EV3 *ev3, int port, QObject *parent = nullptr);

    void setPower(int power);
    void start(bool enable = true);
    void stop();
    void setPolarity(bool straight);
    void setSpeedControlEnabled(bool enable);

signals:
    void powerChanged();

protected:
    int m_port = 0;
    char m_internalPort = 0x00;
    bool m_polarity = true;
    int m_power = 100;
    EV3 *m_ev3 = nullptr;
    bool m_enableSpeedControl = false;
    QList<char> m_ports = { 0x01, 0x02, 0x04, 0x08 };
};

#endif // EV3_MOTOR_H
