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
    auto motorSocket = new IconLabel(Common::Instance()->iconSocket, Common::Instance()->iconSocketActive, QString::number(motorIndex));
    motorSocket->setStyleSheet("color: white; font-weight: bold");
    return motorSocket;
}

Common::Common()
{
    iconSocket = QIcon(":/resources/EV3-socket.svg");
    iconSocketActive = QIcon(":/resources/EV3-socket-active.svg");
}
