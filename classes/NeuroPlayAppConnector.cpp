#include "NeuroPlayAppConnector.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

NeuroPlayAppConnector::NeuroPlayAppConnector(QObject *parent) : QObject(parent)
{
    m_socket = new QWebSocket();
    connect(m_socket, &QWebSocket::connected, [=]() {
        setConnected(true);
    });
    connect(m_socket, &QWebSocket::disconnected, [=]() {
        setConnected(false);
    });

    connect(m_socket, &QWebSocket::textMessageReceived, this, [=](QString txt) {
        QJsonDocument json = QJsonDocument::fromJson(txt.toUtf8());
        QJsonObject resp = json.object();

        auto resultsObject = resp["results"];
        if (resultsObject.isArray())
        {
            auto devices = resultsObject.toArray();
            int index = 0;
            foreach (auto dev, devices)
            {
                if (dev.isObject())
                {
                   parseSingleJson(dev.toObject(), index);
                }
                index ++;
            }
        }
        else
        {
            parseSingleJson(resp, 0);
        }
    });

    m_timer = new QTimer();
    m_timer->setInterval(200);
    connect(m_timer, &QTimer::timeout, [=]() {
        if (m_socket->state() != QAbstractSocket::ConnectedState) {
            m_socket->open(QUrl("ws://localhost:1336"));
        } else {
            m_socket->sendTextMessage(m_multiplayer ? "bcis" : "bci");
        }
    });
}

NeuroPlayAppConnector::~NeuroPlayAppConnector()
{
    stop();
}

void NeuroPlayAppConnector::startSingleUser(int timeoutMs)
{
    start(false, timeoutMs);
}

void NeuroPlayAppConnector::startMultiUser(int timeoutMs)
{
   start(true, timeoutMs);
}

void NeuroPlayAppConnector::start(bool multiplayer, int timeoutMs)
{
    m_multiplayer = multiplayer;
    if (m_timer)
    {
        if (timeoutMs < 10) timeoutMs = 10;
        m_timer->stop();
        m_timer->setInterval(timeoutMs);
        m_timer->start();
    }
}

void NeuroPlayAppConnector::stop()
{
    if (m_timer) m_timer->stop();
}

void NeuroPlayAppConnector::setConnected(bool connected)
{
    if (m_isConnected == connected) return;

    m_isConnected = connected;
    if (connected) emit this->connected();
    else emit this->disconnected();
}

void NeuroPlayAppConnector::parseSingleJson(QJsonObject obj, int userIndex)
{
    float meditation = 0;
    auto med = obj["meditation"];
    if (!med.isNull()) {
        meditation = med.toDouble();
    }

    float concentration = 0;
    auto con = obj["concentration"];
    if (!con.isNull()) {
        concentration = con.toDouble();
    }

    int mentalState = -1;
    auto st = obj["mental_state"];
    if (!st.isNull()) {
        mentalState = st.toInt();
    }

    emit userBCI(userIndex, meditation, concentration, mentalState);
}
