#include "Settings.h"

Settings::Settings(QObject *parent) : QObject{parent}
{
    m_settings = new QSettings("neuroev3.settings", QSettings::Format::IniFormat);

}

bool Settings::getMentalStateEnabled(int state)
{
    QString header = mentalStateHeader(state);
    return m_settings->value(header + "/enabled", false).toBool();
}

void Settings::setMentalStateEnabled(int state, bool enabled)
{
    QString header = mentalStateHeader(state);
    m_settings->setValue(header + "/enabled", enabled);
}

QVector<int> Settings::getMentalStateDrives(int state)
{
    QVector<int> drives;
    QString header = mentalStateHeader(state);
    for (int i = 1; i<=4; i++) {
        drives << m_settings->value(header + "/drive" + QString::number(i), 0).toInt();
    }
    return drives;
}

void Settings::setMentalStateDrives(int state, QVector<int> drives)
{
    QString header = mentalStateHeader(state);
    for (int i = 0; i<drives.length(); i++) {
        m_settings->setValue(header + "/drive" + QString::number(i + 1), drives[i]);
    }
}

QString Settings::mentalStateHeader(int state)
{
    return "mentalState" + QString::number(state);
}
