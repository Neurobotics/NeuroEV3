#include "ComProfile.h"
#include <QStandardPaths>
#include <QSerialPort>

ComProfile::ComProfile(QObject *parent) : QSettings(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/ComSettings.ini", QSettings::Format::IniFormat, parent)
{

}

QString ComProfile::port()
{
    return value(PORTNAME, "").toString();
}

void ComProfile::setPort(QString p)
{
    if (port() == p) return;
    setValue(PORTNAME, p);
    emit portChanged(p);
    emit propertyChanged(PORTNAME);
}

int ComProfile::baudRate()
{
    return value(BAUDRATE, QSerialPort::BaudRate::Baud19200).toInt();
}

void ComProfile::setBaudRate(int rate)
{
    if (baudRate() == rate) return;
    setValue(BAUDRATE, rate);
    emit baudRateChanged(rate);
    emit propertyChanged(BAUDRATE);
}

QSerialPort::DataBits ComProfile::dataBits()
{
    int bits = value(DATABITS, 8).toInt();
    if (bits < 5 || bits > 8) {
        bits = 8;
    }
    return (QSerialPort::DataBits)bits;
}

void ComProfile::setDataBits(int bits)
{
    if (bits < 5 || bits > 8) {
        bits = 8;
    }
    if (dataBits() == bits) return;
    setValue(DATABITS, bits);
    emit dataBitsChanged((QSerialPort::DataBits)bits);
    emit propertyChanged(DATABITS);
}

QSerialPort::Parity ComProfile::parity()
{
    int p = value(PARITY, 2).toInt();
    if (p != 0 && (p < QSerialPort::Parity::EvenParity || p > QSerialPort::Parity::MarkParity)) {
        p = QSerialPort::Parity::EvenParity;
    }
    return (QSerialPort::Parity)p;
}

void ComProfile::setParity(QSerialPort::Parity p)
{
    if (parity() == p) return;

    setValue(PARITY, (int)p);
    emit parityChanged(p);
    emit propertyChanged(PARITY);
}

QString ComProfile::command(QString key)
{
    if (key.isEmpty()) return "";
    return value(COMMAND + key, "").toString();
}

void ComProfile::setCommand(QString key, QString value)
{
    if (key.isEmpty()) return;
    setValue(COMMAND + key, value);
}

int ComProfile::timeoutMs()
{
    return qBound(0, value(TIMEOUT, 0).toInt(), 10000);
}

void ComProfile::setTimeoutMs(int timeMs)
{
    timeMs = qBound(0, timeMs, 10000);
    setValue(TIMEOUT, timeMs);
    emit timeoutChanged(timeMs);
}

QString ComProfile::action(const QString &name)
{
    return value(ACTION + name, "").toString();
}

void ComProfile::setAction(const QString &name, const QString &value)
{
    setValue(ACTION + name, value);
}

QString ComProfile::speedPrefix()
{
    return value(SPEEDPREFIX, "").toString();
}

QString ComProfile::speedSuffix()
{
    return value(SPEEDSUFFIX, ";").toString();
}

void ComProfile::setSpeedPrefix(const QString &prefix)
{
    setValue(SPEEDPREFIX, prefix);
}

void ComProfile::setSpeedSuffix(const QString &suffix)
{
    setValue(SPEEDSUFFIX, suffix);
}
