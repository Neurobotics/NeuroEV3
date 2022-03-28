#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QIcon>
#include "ui/IconLabel.h"

const QString MEDITATION = "meditation";
const QString CONCENTRATION = "concentration";
const QString MENTALSTATE = "mentalState";

const int MAX_MENTAL_STATES = 4;
const int MAX_MOTORS = 4;

class Common {
public:
    static Common* Instance();

    QIcon iconSocket;
    QIcon iconSocketActive;
    QIcon iconConnected;
    QIcon iconDisconnected;

    IconLabel *motorSocket(int motorIndex);
    IconLabel *connectStatusWidget();

    static QString motorString(int motorIndex);

protected:
    Common();
    static Common* _instance;
};




#endif // COMMON_H
