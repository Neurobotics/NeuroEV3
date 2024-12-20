#include "ComProfile.h"
#include <QStandardPaths>
#include <QSerialPort>

ComProfile::ComProfile(QObject *parent) : QSettings(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/ComSettings.ini", QSettings::Format::IniFormat, parent)
{

}

QString ComProfile::getPort()
{
    return value("port", "").toString();
}

void ComProfile::setPort(QString port)
{
    if (getPort() == port) return;
    setValue("port", port);
    emit portChanged(port);
}

int ComProfile::baudRate()
{
    return value("baudRate", QSerialPort::BaudRate::Baud19200).toInt();
}

void ComProfile::setBaudRate(int rate)
{
    if (baudRate() == rate) return;
    setValue("baudRate", rate);
    emit baudRateChanged(rate);
}

QSerialPort::DataBits ComProfile::dataBits()
{
    int bits = value("dataBits", 8).toInt();
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
    setValue("dataBits", bits);
    emit dataBitsChanged((QSerialPort::DataBits)bits);
}

QSerialPort::Parity ComProfile::parity()
{
    int p = value("parity", 2).toInt();
    if (p != 0 && (p < QSerialPort::Parity::EvenParity || p > QSerialPort::Parity::MarkParity)) {
        p = QSerialPort::Parity::EvenParity;
    }
    return (QSerialPort::Parity)p;
}

void ComProfile::setParity(QSerialPort::Parity p)
{
    if (parity() == p) return;

    setValue("parity", (int)p);
    emit parityChanged(p);
}
