#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QMetaEnum>

const QString MEDITATION = "meditation";
const QString CONCENTRATION = "concentration";
const QString MENTALSTATE = "mentalState";
const QString BIOSIGNALSTATE = "biosignalState";

const int MAX_MENTAL_STATES = 4;
const QString MULTIPLAYER = "multiplayer";

class Common {
public:
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

    static QString appVersion(bool withBuild = false);
    static QString OS();

protected:
    Common();
};

#endif // COMMON_H
