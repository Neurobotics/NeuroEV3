#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include "Common.h"
#include "ev3/EV3.h"

class Settings : public QSettings
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);

    EV3::ConnectionType getConnectionType();
    void setConnectionType(EV3::ConnectionType type);

    bool ev3Mode();
    void setEV3mode(bool on);

    bool getMentalStateEnabled(int state, QString prefix = "");
    void setMentalStateEnabled(int state, bool enabled, QString prefix = "");

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
    QString mentalStateHeader(int state);    

};

#endif // SETTINGS_H
