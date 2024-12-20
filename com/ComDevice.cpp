#include "ComDevice.h"

ComDevice::ComDevice(QObject *parent) : QObject(parent)
{
    m_profile = new ComProfile();

    m_port = new QSerialPort();
    connect(m_port, &QSerialPort::readyRead, [=]() {
        auto msg = m_port->readAll();
        qDebug() << "COM << " << msg.toHex(' ');
    });

    reconnect();

    m_connectionTimer = new QTimer(this);
    connect(m_connectionTimer, &QTimer::timeout, this, [=]() {
        if (!isConnected() && m_isEnabled) {
            qDebug() << "Reconnecting";
            reconnect();
        }
    });
    m_connectionTimer->start(1000);
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

void ComDevice::reconnect()
{
    updatePortSettings();
}

bool ComDevice::isConnected()
{
    return m_port && m_port->isOpen();
}

void ComDevice::updatePortSettings()
{
    if (!m_port) return;

    m_port->close();

    emit disconnected();

    m_port->setPortName(m_profile->getPort());
    m_port->setBaudRate(m_profile->baudRate());
    m_port->setParity(m_profile->parity());
    m_port->setDataBits(m_profile->dataBits());

    bool open = m_port->open(QIODevice::ReadWrite);
    if (open) {
        emit connected();
    }
}
