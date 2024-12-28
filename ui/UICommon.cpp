#include "UICommon.h"
#include <QDesktopServices>

UICommon *UICommon::_instance = nullptr;

UICommon::UICommon(QObject *parent) : QObject(parent)
{
    iconSocket = QIcon(":/resources/EV3-socket.svg");
    iconSocketActive = QIcon(":/resources/EV3-socket-active.svg");
    iconConnected = QIcon(":/resources/connected.svg");
    iconDisconnected = QIcon(":/resources/disconnected.svg");
}

UICommon *UICommon::Instance()
{
    if (!_instance) {
        _instance = new UICommon();
    }
    return _instance;
}

QString UICommon::motorString(int motorIndex)
{
    switch (motorIndex) {
    case 1: return "A";
    case 2: return "B";
    case 3: return "C";
    case 4: return "D";
    }
    return "?";
}

QProgressBar *UICommon::progressBar(QString color)
{
    auto progress = new QProgressBar();
    progress->setRange(0, 100);
    progress->setStyleSheet("QProgressBar { text-align: center; border: 2px solid grey; border-radius: 5px; background-color: #FFF; } "
                            "QProgressBar::chunk { background-color: " + color + "; padding: 4px; }");
    return progress;
}

QProgressBar *UICommon::meditationBar()
{
    return progressBar(ColorMeditation);
}

QProgressBar *UICommon::concentrationBar()
{
    return progressBar(ColorConcentration);
}

void UICommon::ChangeBackground(QWidget *w, QColor color)
{
    QPalette pal(w->palette());
    pal.setColor(QPalette::Window, color);
    w->setAutoFillBackground(true);
    w->setPalette(pal);
}

QPushButton *UICommon::iconedButton(QString iconPath, QSize size, QSize iconSize)
{
    auto btn = new QPushButton();
    btn->setIcon(QIcon(iconPath));
    btn->setIconSize(iconSize);
    btn->setFixedSize(size);
    btn->setContentsMargins(0,0,0,0);
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
}

QPushButton *UICommon::flatButton(QString text, QIcon icon, QString tooltip, QUrl url)
{
    auto btn = new QPushButton(icon, text);
    btn->setFlat(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setToolTip(tooltip);
    if (!url.isEmpty()) {
        connect(btn, &QPushButton::clicked, [=]() {
            QDesktopServices::openUrl(url);
        });
    }
    return btn;
}

IconLabel *UICommon::motorSocket(int motorIndex)
{
    auto motorSocket = new IconLabel(iconSocket, iconSocketActive, motorString(motorIndex));
    motorSocket->setStyleSheet("color: white; font-weight: bold; font-size: 12px; padding-right: 0px;");
    return motorSocket;
}

IconLabel *UICommon::connectStatusWidget()
{
    return new IconLabel(iconDisconnected, iconConnected);
}

QColor UICommon::biosignalStateColor(int state)
{
    switch (state) {
    case 1: return state1;
    case 2: return state2;
    case 3: return state3;
    case 4: return state4;
    case 5: return state5;
    case 6: return state6;
    case 7: return state7;
    case 8: return state8;
    default: return stateGray;
    }
}

