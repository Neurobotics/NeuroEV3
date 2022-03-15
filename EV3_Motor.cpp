#include "EV3_Motor.h"

EV3_Motor::EV3_Motor(EV3 *ev3, int port, QObject *parent) : QObject(parent) {
    m_ev3 = ev3;
    m_port = port;
    if (port > 0 && port < 5) {
        m_internalPort = m_ports[port - 1];
    }
}

void EV3_Motor::setPower(int power)
{
    power = qBound(-100, power, 100);

    if (m_internalPort == 0)
        return;

    char command[14];
    command[0] = opOUTPUT_START; // opOUTPUT_POLARITY;
    command[1] = 0x81;
    command[2] = 0x00;
    command[3] = 0x81;
    command[4] = m_internalPort;
    command[5] = 0x81;
    command[6] = m_polarity ? qint8(1) : qint8(-1);

    command[7] = m_enableSpeedControl ? opOUTPUT_SPEED : opOUTPUT_POWER;
    command[8] = 0x81;
    command[9] = 0x00;
    command[10] = 0x81;
    command[11] = m_internalPort;
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

void EV3_Motor::start(bool enable)
{
    if (!enable) stop();
    else {
        char command[5];
        command[0] = opOUTPUT_START;
        command[1] = 0x81;
        command[2] = 0x00;
        command[3] = 0x81;
        command[4] = m_internalPort;
        m_ev3->sendCommand(QByteArray::fromRawData(command, 5));
    }
}

void EV3_Motor::stop()
{
    char command[7];
    command[0] = opOUTPUT_STOP;
    command[1] = 0x81;
    command[2] = 0x00;
    command[3] = 0x81;
    command[4] = m_internalPort;
    command[5] = 0x81;
    command[6] = 0x01; // brake
    m_ev3->sendCommand(QByteArray::fromRawData(command, 7));
}

void EV3_Motor::setPolarity(bool straight)
{
    char command[7];
    command[0] = opOUTPUT_POLARITY;
    command[1] = 0x81;
    command[2] = 0x00;
    command[3] = 0x81;
    command[4] = m_internalPort;
    command[5] = 0x81;
    command[6] = straight ? qint8(1) : qint8(-1);
    m_ev3->sendCommand(QByteArray::fromRawData(command, 7));
}

void EV3_Motor::setSpeedControlEnabled(bool enable)
{
    m_enableSpeedControl = enable;
}
