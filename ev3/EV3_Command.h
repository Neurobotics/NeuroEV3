#ifndef EV3_COMMAND_H
#define EV3_COMMAND_H

#include <QDataStream>
#include <QByteArray>
#include <QIODevice>
#include "EV3_ByteCodes.h"

class EV3_Command
{
public:
    EV3_Command();

    void addOpCode(OP opCode);
    void addParameter(qint8 p);
    void addParameter(qint16 p);
    void addParameter(qint32 p);

    const QByteArray &data() const { return m_data; }

private:
    QByteArray m_data;
    QDataStream m_stream;
};

#endif // EV3_COMMAND_H
