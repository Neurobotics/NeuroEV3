#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "EV3.h"
#include "EV3_Motor.h"
#include <QWebSocket>
#include <QGridLayout>
#include "Settings.h"
#include <QMetaEnum>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    EV3 *m_ev3 = nullptr;
    QWebSocket *socket = nullptr;

    EV3::ConnectionState m_state = EV3::ConnectionState::Disconnected;

    int m_mentalState = -1;
    int meditation = 0;
    int concentration = 0;

    Settings *m_settings = nullptr;

    template <typename T> QString EnumToString(T value)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        return QString(metaEnum.valueToKey(value));
    }
};
#endif // MAINWINDOW_H
