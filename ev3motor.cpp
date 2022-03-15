#include "ev3motor.h"

ev3motor::ev3motor(Ev3 *ev3, int port, QObject *parent) : QObject(parent) {
    m_ev3 = ev3;
    m_port = port;
}

void ev3motor::setPower(int power)
{
    power = qBound(-100, power, 100);

    if (m_power == power || m_port == 0)
        return;

    char command[14];
    command[0] = opOUTPUT_POLARITY;
    command[1] = 0x81;
    command[2] = 0x00;
    command[3] = 0x81;
    command[4] = m_port;
    command[5] = 0x81;
    command[6] = m_polarity ? qint8(1) : qint8(-1);

    command[7] = m_enableSpeedControl? opOUTPUT_SPEED : opOUTPUT_POWER;
    command[8] = 0x81;
    command[9] = 0x00;
    command[10] = 0x81;
    command[11] = m_port;
    command[12] = 0x81;
    command[13] = power;

    m_ev3->sendCommand(QByteArray::fromRawData(command, 14));

    if (power == 0)
        stop();
    else if (m_power == 0)
        start();

    m_power = power;
    emit powerChanged();
}

void ev3motor::start()
{
    char command[5];
    command[0] = opOUTPUT_START;
    command[1] = 0x81;
    command[2] = 0x00;
    command[3] = 0x81;
    command[4] = m_port;
    m_ev3->sendCommand(QByteArray::fromRawData(command, 5));
}

void ev3motor::stop()
{
    char command[7];
    command[0] = opOUTPUT_STOP;
    command[1] = 0x81;
    command[2] = 0x00;
    command[3] = 0x81;
    command[4] = m_port;
    command[5] = 0x81;
    command[6] = 0x01; // brake
    m_ev3->sendCommand(QByteArray::fromRawData(command, 7));
}

void ev3motor::setPolarity(int p)
{
    char command[7];
    command[0] = opOUTPUT_POLARITY;
    command[1] = 0x81;
    command[2] = 0x00;
    command[3] = 0x81;
    command[4] = m_port;
    command[5] = 0x81;
    command[6] = p > 0 ? qint8(1) : qint8(-1);
    m_ev3->sendCommand(QByteArray::fromRawData(command, 7));
}

void ev3motor::setSpeedControlEnabled(bool enable)
{
    m_enableSpeedControl = enable;
}
