#include "Settings.h"
#include <QStandardPaths>

Settings::Settings(QObject *parent) : QSettings(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/app.ini", QSettings::Format::IniFormat, parent)
{

}

EV3::ConnectionType Settings::getConnectionType()
{
    return value("ev3/connection", "wifi").toString() == "wifi" ? EV3::WiFi : EV3::Bluetooth;
}

void Settings::setConnectionType(EV3::ConnectionType type)
{
    setValue("ev3/connection", type == EV3::WiFi ? "wifi" : "bluetooth");
}

bool Settings::ev3Mode()
{
    return value("mode", "ev3").toString() == "ev3";
}

void Settings::setEV3mode(bool on)
{
    setValue("mode", on ? "ev3" : "com");
}

bool Settings::getMentalStateEnabled(int state, QString prefix)
{
    QString header = prefix + biosignalStateHeader(state);
    return value(header + "/enabled", false).toBool();
}

void Settings::setMentalStateEnabled(int state, bool enabled, QString prefix)
{
    QString header = prefix + biosignalStateHeader(state);
    setValue(header + "/enabled", enabled);
}

QVector<int> Settings::getMentalStateDrives(int state)
{
    QVector<int> drives;
    QString header = biosignalStateHeader(state);
    for (int i = 1; i<=4; i++) {
        drives << value(header + "/drive" + QString::number(i), 0).toInt();
    }
    return drives;
}

void Settings::setMentalStateDrives(int state, QVector<int> drives)
{
    QString header = biosignalStateHeader(state);
    for (int i = 0; i<drives.length(); i++) {
        setValue(header + "/drive" + QString::number(i + 1), drives[i]);
    }
}

void Settings::setMentalStateDrive(int state, int motorIndex, int value)
{
    setValue(biosignalStateHeader(state) + "/drive" + QString::number(motorIndex), value);
}

int Settings::getMentalStateDrive(int state, int motorIndex)
{
    return value(biosignalStateHeader(state) + "/drive" + QString::number(motorIndex), 0).toInt();
}

QString Settings::biosignalStateHeader(int state)
{
    return MENTALSTATE + QString::number(state);
}

void Settings::setMetaIndexDriveEnabled(QString metaIndex, int motorIndex, bool enable)
{
     setValue(metaIndex + "/drive" + QString::number(motorIndex) + "/enabled", enable);
}

void Settings::setMetaIndexDriveCoeff(QString metaIndex, int motorIndex, double coeff)
{
     setValue(QString(metaIndex) + "/drive" + QString::number(motorIndex) + "/coeff", coeff);
}

bool Settings::getMetaIndexDriveEnabled(QString metaIndex, int motorIndex)
{
    return value(metaIndex + "/drive" + QString::number(motorIndex) + "/enabled", false).toBool();
}

double Settings::getMetaIndexDriveCoeff(QString metaIndex, int motorIndex)
{
    return value(metaIndex + "/drive" + QString::number(motorIndex) + "/coeff", 1).toDouble();
}

void Settings::setMultiplayerControl(QString control)
{
    setValue("muliplayer/control", control);
}

QString Settings::getMultiplayerControl()
{
    return value("muliplayer/control", "meditation").toString();
}

int Settings::getBiosignalStatesAmountToControl()
{
    return qBound(1, value("bsAmountToControl", 1).toInt(), 10);
}

void Settings::setBiosignalStatesAmountToControl(int value)
{
    setValue("bsAmountToControl", qBound(1, value, 10));
}

int Settings::getBiosignalStatesMax()
{
    return qBound(2, value("bsMax", 3).toInt(), MAX_MENTAL_STATES);
}

void Settings::setBiosignalStatesMax(int value)
{
    setValue("bsMax", qBound(2, value, MAX_MENTAL_STATES));
}

int Settings::getBiosignalStatesWordLength()
{
    return qBound(1, value("bsWord", 2).toInt(), MAX_BIOSIGNAL_STATE_WORD_LENGTH);
}

void Settings::setBiosignalStatesWordLength(int value)
{
    setValue("bsWord", qBound(1, value, MAX_BIOSIGNAL_STATE_WORD_LENGTH));
}
