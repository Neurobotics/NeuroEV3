#include "EV3_Command.h"

EV3_Command::EV3_Command()
    : m_stream(&m_data, QIODevice::WriteOnly)
{
}

void EV3_Command::addOpCode(OP opCode)
{
    m_stream << qint8(opCode);
}

void EV3_Command::addParameter(qint8 p)
{
    m_stream << qint8(0x81) << p;
}

void EV3_Command::addParameter(qint16 p)
{
    m_stream << qint8(0x82) << p;
}

void EV3_Command::addParameter(qint32 p)
{
    m_stream << qint8(0x83) << p;
}
