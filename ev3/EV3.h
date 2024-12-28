#ifndef EV3_H
#define EV3_H

#include <QObject>
#include <QDataStream>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QtBluetooth/QBluetoothSocket>
#include <QBluetoothDeviceDiscoveryAgent>

class EV3_Motor;

const int EV3_MAX_MOTORS = 4;

#define EV3_UDP_PORT 3015
#define EV3_TCP_PORT 5555

class EV3 : public QObject
{
    Q_OBJECT
    Q_ENUMS(ConnectionState)
    Q_PROPERTY(ConnectionState state READ connectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)    
public:
    enum ConnectionType {
        WiFi,
        Bluetooth
    };
    Q_ENUM(ConnectionType)

    enum ConnectionState {
        Unknown,
        Disconnected,
        Disconnecting,
        Searching,
        Connected,
        Connecting
    };
    Q_ENUM(ConnectionState)

    EV3(ConnectionType type = ConnectionType::WiFi, QObject *parent = nullptr);
    ~EV3();

    void setConnectionType(ConnectionType type, bool force = false);


    QVector<EV3_Motor*> motors();
    EV3_Motor* motor(int motorIndex1To4);
    void stopMotors();

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

protected Q_SLOTS:
    void startDeviceDiscovery();
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void readSocket();

    void updateState(QAbstractSocket::SocketState state);
    void updateError(QAbstractSocket::SocketError error);
    void processBroadcastDatagram();

    void close();

private:
    QVector<EV3_Motor*> m_motors;
    bool unlockEv3();
    void clearError();
    void setState(ConnectionState state);

    QString m_serialNumber = "";

    ConnectionState m_connectionState = ConnectionState::Disconnected;
    QString m_error = "";

    ConnectionType m_connectionType = WiFi;

    QUdpSocket *m_broadcast = nullptr;
    QTcpSocket *m_connection = nullptr;
    QBluetoothSocket *m_bluetooth = nullptr;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;

    void startClient(const QBluetoothDeviceInfo &remoteService);
};

#endif // EV3_H
