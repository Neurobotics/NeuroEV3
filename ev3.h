#ifndef EV3_H
#define EV3_H

#include <QObject>
#include <QDataStream>
#include <QTcpSocket>
#include <QUdpSocket>

#define EV3_UDP_PORT 3015
#define EV3_TCP_PORT 5555


class Ev3 : public QObject
{
    Q_OBJECT
    Q_ENUMS(ConnectionState)
    Q_PROPERTY(ConnectionState state READ connectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)    
public:
    enum ConnectionState {
        Unknown,
        Disconnected,
        Disconnecting,
        Searching,
        Connected,
        Connecting
    };
    Q_ENUM(ConnectionState)

    Ev3(QObject *parent = nullptr);
    ~Ev3();
 #pragma pack(push,1)

    struct packetStruct
    {
        qint16 notUse;
        unsigned short id;
        qint8 notUse2;
        float value;
    };
#pragma pack(pop)

    ConnectionState connectionState() const;
    QString error() const { return m_error; }

    void sendCommand(const QByteArray &data, bool noReply = true);
    QByteArray readResponse();
    void sendDirectCommand(const QByteArray &data);

Q_SIGNALS:
    void connectionStateChanged();
    void errorChanged();
    void readyRead(unsigned short id, float value);

public Q_SLOTS:
    void searchAndConnect();
    void disconnect();

private Q_SLOTS:
    void updateState(QAbstractSocket::SocketState state);
    void updateError(QAbstractSocket::SocketError error);
    void processBroadcastDatagram();

private:
    bool unlockEv3();
    void clearError();
    void setState(ConnectionState state);

    QString m_serialNumber = "";

    ConnectionState m_connectionState = ConnectionState::Disconnected;
    QString m_error = "";

    QUdpSocket *m_broadcast = nullptr;
    QTcpSocket *m_connection = nullptr;

};

#endif // EV3_H
