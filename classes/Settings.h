#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include "Common.h"
#include "ev3/EV3.h"

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);

    EV3::ConnectionType getConnectionType();
    void setConnectionType(EV3::ConnectionType type);

    bool getMentalStateEnabled(int state);
    void setMentalStateEnabled(int state, bool enabled);

    QVector<int> getMentalStateDrives(int state);
    void setMentalStateDrives(int state, QVector<int> drives);
    void setMentalStateDrive(int state, int motorIndex, int value);
    int getMentalStateDrive(int state, int motorIndex);

    void setMetaIndexDriveEnabled(QString metaIndex, int motorIndex, bool enable);
    void setMetaIndexDriveCoeff(QString metaIndex, int motorIndex, double coeff);
    bool getMetaIndexDriveEnabled(QString metaIndex, int motorIndex);
    double getMetaIndexDriveCoeff(QString metaIndex, int motorIndex);

    void setMultiplayerControl(QString control);
    QString getMultiplayerControl();

protected:
    QSettings *m_settings = nullptr;
    QString mentalStateHeader(int state);    

};

#endif // SETTINGS_H
