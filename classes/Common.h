#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QIcon>
#include "ui/IconLabel.h"
#include <QMetaEnum>

const QString MEDITATION = "meditation";
const QString CONCENTRATION = "concentration";
const QString MENTALSTATE = "mentalState";

const int MAX_MENTAL_STATES = 4;
const int MAX_MOTORS = 4;
const QString MULTIPLAYER = "multiplayer";

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

    template <typename T> static QString EnumToString(T value)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        return QString(metaEnum.valueToKey(value));
    }

    template <typename T> static QStringList EnumTextValues()
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        QStringList values;
        for (int i = 0, n = metaEnum.keyCount(); i<n; i++) {
            values << QString(metaEnum.key(i));
        }
        return values;
    }

    template <typename T> static QList<int> EnumIntValues()
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        QList<int> values;
        for (int i = 0, n = metaEnum.keyCount(); i<n; i++) {
            values << metaEnum.value(i);
        }
        return values;
    }

protected:
    Common();
    static Common* _instance;
};

#endif // COMMON_H
