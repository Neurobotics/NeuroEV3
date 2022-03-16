#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

const QString MEDITATION = "meditation";
const QString CONCENTRATION = "concentration";
const QString MENTALSTATE = "mentalState";

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);

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

protected:
    QSettings *m_settings = nullptr;
    QString mentalStateHeader(int state);    

};

#endif // SETTINGS_H
