#ifndef COMPROFILE_H
#define COMPROFILE_H

#include <QObject>
#include <QSettings>
#include <QSerialPort>

class ComProfile : public QSettings
{
    Q_OBJECT
public:
    explicit ComProfile(QObject *parent = nullptr);

    QString getPort();
    void setPort(QString port);

    int baudRate();
    void setBaudRate(int rate);

    QSerialPort::DataBits dataBits();
    void setDataBits(int bits);

    QSerialPort::Parity parity();
    void setParity(QSerialPort::Parity p);

signals:
    void portChanged(QString port);
    void baudRateChanged(int baudRate);
    void dataBitsChanged(QSerialPort::DataBits dataBits);
    void parityChanged(QSerialPort::Parity parity);
};

#endif // COMPROFILE_H
