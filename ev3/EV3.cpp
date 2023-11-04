#include "EV3.h"
#include "EV3_Motor.h"
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QDebug>


#define UNLOCK_MESSAGE "GET /target?sn=%1 VMTP1.0\nProtocol: EV3"

void EV3::startDeviceDiscovery()
{

    // Create a discovery agent and connect to its signals
    if (!discoveryAgent) {
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    }

    // Start a discovery
    discoveryAgent->start();

    //...
}


void EV3::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')';

    if (device.name().startsWith("EV3")) {
        startClient(device);
        discoveryAgent->stop();
    }
}

void EV3::readSocket() {
    if (m_connectionState != Connected)
        return;

    QByteArray ba = m_bluetooth ? m_bluetooth->readAll() : m_connection->readAll();
    packetStruct *packet = reinterpret_cast<packetStruct *>(ba.data());
    qDebug() << "READ" << ba.toHex(',');
    qDebug() << packet->id << " id " << packet->value<<" value ";  // пока не удаляйте
    emit readyRead(packet->id,packet->value);
}

EV3::EV3(ConnectionType type, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<EV3::ConnectionType>("ConnectionType");
    qRegisterMetaType<EV3::ConnectionState>("ConnectionState");

    for (int i = 1; i<=4; i++) {
        m_motors << new EV3_Motor(this, i);
    }

    setConnectionType(type);
}

void EV3::setConnectionType(ConnectionType type)
{
    close();

    qDebug() << "Start EV3 via" << type;

    if (type == WiFi)
    {
        m_broadcast = new QUdpSocket;
        QObject::connect(m_broadcast, SIGNAL(readyRead()), this, SLOT(processBroadcastDatagram()));

        m_connection = new QTcpSocket;
        connect(m_connection, &QTcpSocket::stateChanged, this, &EV3::updateState);
        connect(m_connection, &QTcpSocket::readyRead,[=]() {

        });
        connect(m_connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(updateError(QAbstractSocket::SocketError)));
    }
    else if (type == Bluetooth)
    {
        QBluetoothLocalDevice localDevice;
        if (localDevice.isValid()) localDevice.powerOn();
        startDeviceDiscovery();
    }
}

EV3::~EV3()
{
    close();
}

QVector<EV3_Motor *> EV3::motors()
{
    return m_motors;
}

EV3_Motor *EV3::motor(int motorIndex1To4)
{
    if (motorIndex1To4 > 0 && motorIndex1To4 <= 4) {
        return m_motors[motorIndex1To4 - 1];
    }
    return nullptr;
}

void EV3::stopMotors()
{
    foreach (auto motor, m_motors) {
        motor->stop();
    }
}

void EV3::searchAndConnect()
{
    if (m_connectionState > Disconnected)
        return;

    setState(Searching);

    m_serialNumber.clear();
    m_broadcast->bind(3015);
}

void EV3::processBroadcastDatagram()
{
    clearError();

    if (!m_broadcast) return;

    QHostAddress address;
    quint16 udpPort = -1;

    while (m_broadcast->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_broadcast->pendingDatagramSize());
        m_broadcast->readDatagram(datagram.data(), datagram.size(), &address, &udpPort);
        qDebug() << "Broadcast from" << address.toString() << "on port" << udpPort;
        m_serialNumber = datagram.mid(15, 12);
    }

    // Send handshake
    m_broadcast->writeDatagram(QByteArray("1"), address, udpPort);
    m_broadcast->close();

    // Establish connection
    setState(Connecting);
    qDebug() << "Connecting to" << address.toString() << "(SN=" << m_serialNumber << ") on port 5555";
    m_connection->connectToHost(address, 5555);
}

void EV3::close()
{
    disconnect();
    if (m_connection) {
        m_connection->deleteLater();
        m_connection = nullptr;
    }
    if (m_broadcast) {
        m_broadcast->deleteLater();
        m_broadcast = nullptr;
    }
    if (m_bluetooth) {
        m_bluetooth->deleteLater();
        m_bluetooth = nullptr;
    }
    if (discoveryAgent) {
        discoveryAgent->deleteLater();
        discoveryAgent->deleteLater();
    }
}

bool EV3::unlockEv3()
{
    if (m_connection) {
        qDebug() << "Sending unlock message";
        m_connection->write(QString(QLatin1String(UNLOCK_MESSAGE)).arg(m_serialNumber).toLatin1());
        if (m_connection->waitForReadyRead(1000) && m_connection->read(16).startsWith("Accept")) {
            qDebug() << "Connection confirmed";
            return true;
        } else {
            qDebug() << "Unlock failed";
            return false;
        }
    }
    return true;
}

void EV3::disconnect()
{
    stopMotors();

    m_serialNumber.clear();

    if (m_connectionState == Searching) {
        m_broadcast->close();
        setState(Disconnected);
        return;
    }

    if (m_connectionState != Connected)
        return;

    if (m_connection) m_connection->disconnectFromHost();
    if (m_bluetooth) m_bluetooth->disconnectFromService();
}


#include <QDateTime>
void EV3::sendCommand(const QByteArray &data, bool noReply)
{
    if (!m_bluetooth) {
        if (m_connectionState != Connected)
            return;
    }

    qDebug() << QDateTime::currentDateTime() << "cdm" << data.toHex(' ');

    QByteArray actual;
    QDataStream out(&actual, QIODevice::WriteOnly);
    out << quint8(data.size() + 5) << quint8(0)
        << quint8(0) << quint8(0)
        << (noReply ? quint8(0x80) : quint8(0))
        << quint8(0) << quint8(0);

    out.writeRawData(data.constData(), data.size());

    if (m_connection) m_connection->write(actual);
    if (m_bluetooth) {
//        qDebug() << "BL send" << actual.toHex(' ');
        m_bluetooth->write(actual);
    }
}

void EV3::sendDirectCommand(const QByteArray &data)
{
    if (m_connection) m_connection->write(data);
    if (m_bluetooth) m_bluetooth->write(data);
}

QByteArray EV3::readResponse()
{
    if (m_connectionState != Connected)
        return QByteArray();

    if (m_connection) {
        if (!m_connection->bytesAvailable())
            m_connection->waitForReadyRead(1000);
         return m_connection->readAll();
    }
    return QByteArray();
}

EV3::ConnectionState EV3::connectionState() const
{
    return m_connectionState;
}

void EV3::setState(ConnectionState state)
{
    if (m_connectionState == state)
        return;

    if (state == Connected) {
        if (!unlockEv3()) {
            m_connectionState = Disconnected;
            emit connectionStateChanged();
            m_error = QStringLiteral("Could not unlock EV3.");
            emit errorChanged();
            return;
        }
        stopMotors();
    }

    m_connectionState = state;
    emit connectionStateChanged();
}

void EV3::startClient(const QBluetoothDeviceInfo &remoteService)
{
    if (m_bluetooth)
        return;

    m_bluetooth = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    m_bluetooth->connectToService(remoteService.address(), remoteService.serviceUuids()[0]);

    connect(m_bluetooth, &QBluetoothSocket::stateChanged, [=](QBluetoothSocket::SocketState state) {
        updateState((QAbstractSocket::SocketState)state);
    });
    connect(m_bluetooth, &QBluetoothSocket::readyRead, this, &EV3::readSocket);
//    connect(m_bluetooth, &QBluetoothSocket::connected, this, [=]() {
//        qDebug() << "BL connected";
//    });

//    connect(m_bluetooth, &QBluetoothSocket::disconnected, this, [=]() {
//        qDebug() << "BL DIsconnected";
//    });
//    connect(m_bluetooth, &QBluetoothSocket::errorOccurred, this, [=](QBluetoothSocket::SocketError err) {
//        qDebug() << "BL ERR" << err;
//    });
}

void EV3::updateState(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        setState(Disconnected);
        break;
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
        setState(Connecting);
        break;
    case QAbstractSocket::ConnectedState:
    case QAbstractSocket::BoundState:
        setState(Connected);
        clearError();
        break;
    case QAbstractSocket::ClosingState:
        setState(Disconnecting);
        break;
    default:
        setState(Unknown);
    }
}

void EV3::updateError(QAbstractSocket::SocketError error)
{
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        m_error = QStringLiteral("Connection refused.");
        break;
    case QAbstractSocket::RemoteHostClosedError:
        m_error = QStringLiteral("Ev3 closed the connection.");
        break;
    case QAbstractSocket::HostNotFoundError:
        m_error = QStringLiteral("Host not found.");
        break;
    case QAbstractSocket::SocketAccessError:
        m_error = QStringLiteral("Socket access error.");
        break;
    case QAbstractSocket::SocketResourceError:
        m_error = QStringLiteral("Socket resource error.");
        break;
    case QAbstractSocket::SocketTimeoutError:
        m_error = QStringLiteral("Socket operation timed out.");
        break;
    case QAbstractSocket::DatagramTooLargeError:
        m_error = QStringLiteral("Datagram too large.");
        break;
    case QAbstractSocket::NetworkError:
        m_error = QStringLiteral("Network error.");
        break;
    case QAbstractSocket::AddressInUseError:
        m_error = QStringLiteral("Address already in use.");
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        m_error = QStringLiteral("Address not available.");
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        m_error = QStringLiteral("Unsupported socket operation.");
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        m_error = QStringLiteral("Unfinished socket operation.");
        break;
    case QAbstractSocket::OperationError:
        m_error = QStringLiteral("Operation error.");
        break;
    case QAbstractSocket::TemporaryError:
        m_error = QStringLiteral("Temporary error.");
        break;
    case QAbstractSocket::UnknownSocketError:
        m_error = QStringLiteral("Unknown error.");
        break;
    default:
        return;
    }

    emit errorChanged();
}

void EV3::clearError()
{
    if (m_error.isEmpty()) return;

    m_error.clear();
    emit errorChanged();
}
