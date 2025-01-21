#ifndef COMDEVICE_H
#define COMDEVICE_H

#include <QObject>
#include <QSerialPort>
#include "ComProfile.h"
#include <QTimer>

struct ComDeviceCommand {
    QString key = "";
    QString title = "";
    QString symbol = "";

    ComDeviceCommand(QString key, QString title, QString symbol = "")
    {
        this->key = key;
        this->title = title;
        this->symbol = symbol;
    }
};

class ComDevice : public QObject
{
    Q_OBJECT
public:
    explicit ComDevice(QObject *parent = nullptr);

    void setEnabled(bool on);

    ComProfile *profile() const;

    void stop();

    void sendMessage(const QString &msg);
    void sendCommand(const QString &command);
    void sendSpeed(int speed);

    bool performAction(const QString &name);
    QString action(const QString &name);
    void setAction(const QString &name, const QString &value);

    void reconnect();

    bool isConnected();

    static QList<ComDeviceCommand> Commands();

    static ComDeviceCommand CommandByName(const QString &key);

    static inline QString FORWARD = "Forward";
    static inline QString BACKWARDS = "Backwards";
    static inline QString STOP = "Stop";
    static inline QString TURNLEFT = "TurnLeft";
    static inline QString TURNRIGHT = "TurnRight";
    static inline QString CUSTOM1 = "Custom1";
    static inline QString CUSTOM2 = "Custom2";
    static inline QString CUSTOM3 = "Custom3";
    static inline QString CUSTOM4 = "Custom4";
    static inline QString CUSTOM5 = "Custom5";
    static inline QString CUSTOM6 = "Custom6";


signals:
    void connected();
    void disconnected();

protected:
    int m_limitMessageTimeoutMs = 1000;
    qint64 m_lastMessage = 0;
    QTimer *m_messageTimer = nullptr;

    ComProfile *m_profile = nullptr;
    QSerialPort *m_port = nullptr;
    QTimer *m_connectionTimer = nullptr;

    bool m_isEnabled = false;
    void updatePortSettings();

    QString m_message;


    void _sendMessage(const QString &msg);
};

#endif // COMDEVICE_H
