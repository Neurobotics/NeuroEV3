#ifndef COMDEVICE_H
#define COMDEVICE_H

#include <QObject>
#include <QSerialPort>
#include "ComProfile.h"
#include <QTimer>

class ComDevice : public QObject
{
    Q_OBJECT
public:
    explicit ComDevice(QObject *parent = nullptr);

    void setEnabled(bool on);

    ComProfile *profile() const;

    void sendMessage(QString msg);
    void sendCommand(QString command);

    void reconnect();

    bool isConnected();

signals:
    void connected();
    void disconnected();

protected:
    ComProfile *m_profile = nullptr;
    QSerialPort *m_port = nullptr;
    QTimer *m_connectionTimer = nullptr;

    bool m_isEnabled = false;
    void updatePortSettings();
};

#endif // COMDEVICE_H
