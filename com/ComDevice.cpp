#include "ComDevice.h"
#include <QCoreApplication>
#include <QDateTime>

ComDevice::ComDevice(QObject *parent) : QObject(parent)
{
    m_profile = new ComProfile();
    connect(m_profile, &ComProfile::propertyChanged, [=]() {
        updatePortSettings();
    });

    m_limitMessageTimeoutMs = m_profile->timeoutMs();
    connect(m_profile, &ComProfile::timeoutChanged, [=](int timeMs) {
        m_limitMessageTimeoutMs = timeMs;
    });

    m_port = new QSerialPort();
    connect(m_port, &QSerialPort::readyRead, [=]() {
        auto msg = m_port->readAll();
        qDebug() << "COM MESSAGE <<< " << msg.toHex(' ') << QString::fromLatin1(msg);
    });

    connect(m_port, &QSerialPort::errorOccurred, [=](QSerialPort::SerialPortError err) {
        if (err != QSerialPort::NoError) {
            qWarning() << "SerialPort Error: " << err;
        }
    });

    reconnect();

    m_connectionTimer = new QTimer(this);
    connect(m_connectionTimer, &QTimer::timeout, this, [=]() {
        if (!isConnected() && m_isEnabled) {
            qDebug() << "Reconnecting" << m_profile->port() << m_profile->parity() << m_profile->baudRate();
            reconnect();
        }
    });
    m_connectionTimer->start(5000);

    m_messageTimer = new QTimer(this);
    connect(m_messageTimer, &QTimer::timeout, this, [=]() {
        if (isConnected() && !m_message.isEmpty()) {
            _sendMessage(m_message);
            m_message = "";
        }
    });
    m_messageTimer->start(1000);
}

void ComDevice::setEnabled(bool on)
{
    m_isEnabled = on;
    if (!on) {
        m_port->close();
    } else {
        reconnect();
    }
}

ComProfile *ComDevice::profile() const
{
    return m_profile;
}

void ComDevice::stop()
{
    sendCommand(STOP);
}

void ComDevice::sendMessage(const QString &msg)
{
    if (m_limitMessageTimeoutMs == 0) {
        _sendMessage(msg);
    } else {
        qint64 ts = QDateTime::currentMSecsSinceEpoch();
        if (m_lastMessage == 0 || ts - m_lastMessage > m_limitMessageTimeoutMs) {
            _sendMessage(msg);
        } else {
            qDebug() << "Too fast";
            m_message = msg;
        }
    }
}

void ComDevice::_sendMessage(const QString &msg)
{
    if (m_port && m_port->isOpen()) {
        if (m_limitMessageTimeoutMs) {
            m_lastMessage = QDateTime::currentMSecsSinceEpoch();
            m_messageTimer->stop();
            m_messageTimer->start();
        }
        qDebug() << QDateTime::currentDateTime() << "COM >>" << msg;
        m_port->write(msg.toUtf8());
        m_port->waitForBytesWritten();
    }
}

void ComDevice::sendCommand(const QString &command)
{
    QString cmd = m_profile->command(command);
    if (!cmd.isEmpty()) {
        sendMessage(cmd);
    }
}

void ComDevice::sendSpeed(int speed)
{
    sendMessage(m_profile->speedPrefix() + QString::number(qBound(-100, speed, 100), 'f', 0) + m_profile->speedSuffix());
}

bool ComDevice::performAction(const QString &name)
{
    auto a = action(name);
    if (!a.isEmpty()) {
        auto cmd = m_profile->command(a);
        if (!cmd.isEmpty()) {
            sendMessage(cmd);
        }
        return true;
    }
    return false;
}

QString ComDevice::action(const QString &name)
{
    return m_profile->action(name);
}

void ComDevice::setAction(const QString &name, const QString &value)
{
    m_profile->setAction(name, value);
}

void ComDevice::reconnect()
{
    updatePortSettings();
}

bool ComDevice::isConnected()
{
    return m_port && m_port->isOpen();
}

QList<ComDeviceCommand> ComDevice::Commands()
{
    static QList<ComDeviceCommand> coms = {
        ComDeviceCommand(FORWARD, QCoreApplication::translate("Generic", "Forward"), "↑"),
        ComDeviceCommand(BACKWARDS, QCoreApplication::translate("Generic", "Backwards"), "↓"),
        ComDeviceCommand(STOP, QCoreApplication::translate("Generic", "Stop"), "Х"),
        ComDeviceCommand(TURNLEFT, QCoreApplication::translate("Generic", "TurnLeft"), "←"),
        ComDeviceCommand(TURNRIGHT, QCoreApplication::translate("Generic", "TurnRight"), "→"),
        ComDeviceCommand(CUSTOM1, QCoreApplication::translate("Generic", "Custom1")),
        ComDeviceCommand(CUSTOM2, QCoreApplication::translate("Generic", "Custom2")),
        ComDeviceCommand(CUSTOM3, QCoreApplication::translate("Generic", "Custom3"))
    };

    return coms;
}

void ComDevice::updatePortSettings()
{
    if (!m_port) return;

    m_port->close();

    emit disconnected();

    m_port->setPortName(m_profile->port());
    m_port->setBaudRate(m_profile->baudRate());
    m_port->setParity(m_profile->parity());
    m_port->setDataBits(m_profile->dataBits());

    bool open = m_port->open(QIODevice::ReadWrite);
    if (open) {
        emit connected();
    }
}
