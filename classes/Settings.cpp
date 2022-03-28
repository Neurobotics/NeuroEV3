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

void Settings::setMentalStateDrive(int state, int motorIndex, int value)
{
    m_settings->setValue(mentalStateHeader(state) + "/drive" + QString::number(motorIndex), value);
}

int Settings::getMentalStateDrive(int state, int motorIndex)
{
    return m_settings->value(mentalStateHeader(state) + "/drive" + QString::number(motorIndex), 0).toInt();
}

QString Settings::mentalStateHeader(int state)
{
    return MENTALSTATE + QString::number(state);
}

void Settings::setMetaIndexDriveEnabled(QString metaIndex, int motorIndex, bool enable)
{
     m_settings->setValue(metaIndex + "/drive" + QString::number(motorIndex) + "/enabled", enable);
}

void Settings::setMetaIndexDriveCoeff(QString metaIndex, int motorIndex, double coeff)
{
     m_settings->setValue(QString(metaIndex) + "/drive" + QString::number(motorIndex) + "/coeff", coeff);
}

bool Settings::getMetaIndexDriveEnabled(QString metaIndex, int motorIndex)
{
    return m_settings->value(metaIndex + "/drive" + QString::number(motorIndex) + "/enabled", false).toBool();
}

double Settings::getMetaIndexDriveCoeff(QString metaIndex, int motorIndex)
{
    return m_settings->value(metaIndex + "/drive" + QString::number(motorIndex) + "/coeff", 1).toDouble();
}
