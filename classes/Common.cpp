#include "Common.h"

Common *Common::_instance = nullptr;

Common *Common::Instance()
{
    if (!_instance) {
        _instance = new Common();
    }
    return _instance;
}

IconLabel *Common::motorSocket(int motorIndex)
{
    auto motorSocket = new IconLabel(iconSocket, iconSocketActive, motorString(motorIndex));
    motorSocket->setStyleSheet("color: white; font-weight: bold");
    return motorSocket;
}

IconLabel *Common::connectStatusWidget()
{
    return new IconLabel(iconDisconnected, iconConnected);
}

QString Common::motorString(int motorIndex)
{
    switch (motorIndex) {
    case 1: return "A";
    case 2: return "B";
    case 3: return "C";
    case 4: return "D";
    }
    return "?";
}

Common::Common()
{
    iconSocket = QIcon(":/resources/EV3-socket.svg");
    iconSocketActive = QIcon(":/resources/EV3-socket-active.svg");
    iconConnected = QIcon(":/resources/connected.svg");
    iconDisconnected = QIcon(":/resources/disconnected.svg");
}
