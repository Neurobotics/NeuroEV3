#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QIcon>
#include "IconLabel.h"

const QString MEDITATION = "meditation";
const QString CONCENTRATION = "concentration";
const QString MENTALSTATE = "mentalState";

class Common {
public:
    static Common* Instance();

    QIcon iconSocket;
    QIcon iconSocketActive;

    IconLabel *motorSocket(int motorIndex);

protected:
    Common();
    static Common* _instance;
};




#endif // COMMON_H
