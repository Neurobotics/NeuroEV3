#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);

    bool getMentalStateEnabled(int state);
    void setMentalStateEnabled(int state, bool enabled);

    QVector<int> getMentalStateDrives(int state);
    void setMentalStateDrives(int state, QVector<int> drives);

signals:

protected:
    QSettings *m_settings = nullptr;
    QString mentalStateHeader(int state);

};

#endif // SETTINGS_H
