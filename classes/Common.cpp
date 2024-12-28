#include "Common.h"
#include <QCoreApplication>
#include <QSysInfo>

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
    motorSocket->setStyleSheet("color: white; font-weight: bold; font-size: 12px; padding-right: 0px;");
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

QString Common::appVersion(bool withBuild)
{
    QStringList versions = QCoreApplication::applicationVersion().split(".");
    int m = withBuild ? 3 : 2;
    while (versions.length() > m) {
        versions.removeLast();
    }
    return versions.join(".");
}

QString Common::OS()
{
    static QString _os = "";
    if (_os.isEmpty()) {
        auto os = QSysInfo::productType().toLower();

        if (os.contains("windows") || os.contains("winrt")) _os = "windows";

        else if (os.contains("android")) _os = "android";

        else if (os.contains("osx") || os.contains("mac")) _os = "macos";

        else if (os.contains("ios")) _os = "ios";
        else if (os.contains("tvos")) _os = "tvos";
        else if (os.contains("watchos")) _os = "watchos";
        else if (os.contains("ubuntu") ||
                 os.contains("linux") ||
                 os.contains("manjaro") ||
                 os.contains("centos") ||
                 os.contains("fedora") ||
                 os.contains("raspberry")) {
            _os = "linux";
        }
        else _os = "unix";
    }
    return _os;
}

Common::Common()
{
    iconSocket = QIcon(":/resources/EV3-socket.svg");
    iconSocketActive = QIcon(":/resources/EV3-socket-active.svg");
    iconConnected = QIcon(":/resources/connected.svg");
    iconDisconnected = QIcon(":/resources/disconnected.svg");
}
