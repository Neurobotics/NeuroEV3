#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ev3/EV3.h"
#include "ev3/EV3_Motor.h"
#include <QWebSocket>
#include <QGridLayout>
#include "classes/Settings.h"
#include <QMetaEnum>
#include <QPushButton>

struct UserBCI
{
    int mentalState = -1;
    int meditation = 0;
    int concentration = 0;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum ControlState {
        Manual,
        MentalState,
        Meditation,
        Concentration
    };

    QString appVersion(bool withBuild = false);
    QString OS();

protected:
    EV3 *m_ev3 = nullptr;
    QWebSocket *socket = nullptr;

    EV3::ConnectionState m_state = EV3::ConnectionState::Disconnected;

    QList<UserBCI> m_userBCI;

    ControlState m_controlState = Manual;
    bool m_canControl = true;

    Settings *m_settings = nullptr;

    bool m_multiplayer = false;

    QWidget *newVersionButton();
    QPushButton *flatButton(QString text, QIcon icon = QIcon(), QString tooltip = "", QUrl url = QUrl());

    template <typename T> QString EnumToString(T value)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        return QString(metaEnum.valueToKey(value));
    }

    void control();
};
#endif // MAINWINDOW_H
