#ifndef NEUROPLAYAPPCONNECTOR_H
#define NEUROPLAYAPPCONNECTOR_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>

class NeuroPlayAppConnector : public QObject
{
    Q_OBJECT
public:
    explicit NeuroPlayAppConnector(QObject *parent = nullptr);
    ~NeuroPlayAppConnector();

    void startSingleUser(int timeoutMs = 200);
    void startMultiUser(int timeoutMs = 200);
    void start(bool multiplayer = false, int timeoutMs = 200);
    void stop();

signals:
    void connected();
    void disconnected();
    void userBCI(int userIndex, float meditation, float concentration, int biosignalState);
    void userBiosignalState(int userIndex, int state);
    void userPairBCI(QPair<float, float> meditation, QPair<float, float> concentration);

protected:
    bool m_isConnected = false;
    QWebSocket *m_socket = nullptr;
    QTimer *m_timer = nullptr;
    bool m_multiplayer = false;

    qint64 m_biosignalStateTimestamp = 0;

    void setConnected(bool connected);
    QPair<float, float> parseSingleJson(QJsonObject obj, int userIndex = 0);

};

#endif // NEUROPLAYAPPCONNECTOR_H
