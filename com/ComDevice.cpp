#include "ComDevice.h"
#include <QCoreApplication>

ComDevice::ComDevice(QObject *parent) : QObject(parent)
{
    m_profile = new ComProfile();
    connect(m_profile, &ComProfile::propertyChanged, [=]() {
        updatePortSettings();
    });

    m_port = new QSerialPort();
    connect(m_port, &QSerialPort::readyRead, [=]() {
        auto msg = m_port->readAll();
        qDebug() << "COM MESSAGE <<< " << msg.toHex(' ') << QString::fromLatin1(msg);
    });

    reconnect();

    m_connectionTimer = new QTimer(this);
    connect(m_connectionTimer, &QTimer::timeout, this, [=]() {
        if (!isConnected() && m_isEnabled) {
            qDebug() << "Reconnecting";
            reconnect();
        }
    });
    m_connectionTimer->start(5000);
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

void ComDevice::sendMessage(QString msg)
{
    if (m_port && m_port->isOpen()) {
        qDebug() << "COM >>" << msg;
        m_port->write(msg.toUtf8());
        m_port->waitForBytesWritten();
    }
}

void ComDevice::sendCommand(QString command)
{
    QString cmd = m_profile->command(command);
    if (!cmd.isEmpty()) {
        sendMessage(cmd);
    }
}

void ComDevice::sendSpeed(int speed)
{
    sendMessage(QString::number(speed, 'f', 0) + ";");
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
