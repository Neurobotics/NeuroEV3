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

    QString port();
    void setPort(QString port);

    int baudRate();
    void setBaudRate(int rate);

    QSerialPort::DataBits dataBits();
    void setDataBits(int bits);

    QSerialPort::Parity parity();
    void setParity(QSerialPort::Parity p);

    QString command(QString key);
    void setCommand(QString key, QString value);

    int timeoutMs();
    void setTimeoutMs(int timeMs);

    inline static QString BAUDRATE = "baudRate";
    inline static QString PARITY = "parity";
    inline static QString DATABITS = "dataBits";
    inline static QString PORTNAME = "port";
    inline static QString COMMAND = "command";
    inline static QString ACTION = "action";
    inline static QString TIMEOUT = "timeout";

    QString action(const QString &name);
    void setAction(const QString &name, const QString &value);

signals:
    void portChanged(QString port);
    void baudRateChanged(int baudRate);
    void dataBitsChanged(QSerialPort::DataBits dataBits);
    void parityChanged(QSerialPort::Parity parity);
    void propertyChanged(const QString &name);
    void timeoutChanged(int timeoutMs);
};

#endif // COMPROFILE_H
