#include "EV3.h"
#include "EV3_Motor.h"

#include <QDebug>

#define UNLOCK_MESSAGE "GET /target?sn=%1 VMTP1.0\nProtocol: EV3"

EV3::EV3(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<EV3::ConnectionState>("ConnectionState");

    m_broadcast = new QUdpSocket;
    QObject::connect(m_broadcast, SIGNAL(readyRead()), this, SLOT(processBroadcastDatagram()));

    m_connection = new QTcpSocket;
    connect(m_connection, &QTcpSocket::stateChanged, this, &EV3::updateState);
    connect(m_connection, &QTcpSocket::readyRead,[=]() {
        if (m_connectionState != Connected)
            return;
        QByteArray ba = m_connection->readAll();
        packetStruct *packet = reinterpret_cast<packetStruct *>(ba.data());
        //        qDebug() << ba.toHex(',');
        //        qDebug() << packet->id << " id " << packet->value<<" value ";  // пока не удаляйте
        emit readyRead(packet->id,packet->value);
    });
    connect(m_connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(updateError(QAbstractSocket::SocketError)));

    for (int i = 1; i<=4; i++) {
        m_motors << new EV3_Motor(this, i);
    }
}

EV3::~EV3()
{  
    disconnect();
    m_connection->deleteLater();
    m_broadcast->deleteLater();
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

bool EV3::unlockEv3()
{
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

    m_connection->disconnectFromHost();
}

void EV3::sendCommand(const QByteArray &data, bool noReply)
{
    if (m_connectionState != Connected)
        return;

    QByteArray actual;
    QDataStream out(&actual, QIODevice::WriteOnly);
    out << quint8(data.size() + 5) << quint8(0)
        << quint8(0) << quint8(0)
        << (noReply ? quint8(0x80) : quint8(0))
        << quint8(0) << quint8(0);

    out.writeRawData(data.constData(), data.size());

    //    QByteArray ba;
    //    QDataStream st(&ba, QIODevice::WriteOnly);

    //    st << quint8(0x0D)<<quint8(0)<<quint8(1)<<quint8(3)<<quint8(0)<<quint8(4)
    //       <<quint8(0)<<quint8(0x99)
    //       <<quint8(0x1D)<<quint8(0)<<quint8(0)<<quint8(0)<<quint8(0x02)<<quint8(0x01)<<quint8(0x60);

    //    QByteArray result = readResponse();
    //    qDebug()<<result.toHex(',');//QString::fromStdString(result.toStdString());

    //    qDebug() << actual.toHex();

    //    qDebug()<<data.toHex(',');
    m_connection->write(actual);
}

void EV3::sendDirectCommand(const QByteArray &data)
{
    m_connection->write(data);
}

QByteArray EV3::readResponse()
{
    if (m_connectionState != Connected)
        return QByteArray();

    if (!m_connection->bytesAvailable())
        m_connection->waitForReadyRead(1000);
    return m_connection->readAll();
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
