#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMetaEnum>
#include <QPushButton>
#include <QMainWindow>
#include <QGridLayout>
#include <QTabWidget>

#include "ev3/EV3.h"
#include "classes/Settings.h"
#include "classes/NeuroPlayAppConnector.h"

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

    enum DeviceMode {
        Device_EV3,
        Device_COM
    };
    Q_ENUM(DeviceMode)

    enum ControlState {
        Manual,
        MentalState,
        Meditation,
        Concentration
    };
    Q_ENUM(ControlState)

    QString appVersion(bool withBuild = false);
    QString OS();

    static void ChangeBackground(QWidget *w, QColor color);

    QString autoDetectLanguage(QString settingsFile = "");

protected:
    void closeEvent(QCloseEvent *) override;

    DeviceMode m_deviceMode = Device_EV3;

    EV3 *m_ev3 = nullptr;
    EV3::ConnectionState m_state = EV3::ConnectionState::Disconnected;

    QList<UserBCI> m_userBCI;

    ControlState m_controlState = Manual;
    bool m_canControl = true;

    Settings *m_settings = nullptr;

    bool m_multiplayer = false;

    NeuroPlayAppConnector *m_neuroplayConnector = nullptr;

    QWidget *newVersionButton();
    QPushButton *flatButton(QString text, QIcon icon = QIcon(), QString tooltip = "", QUrl url = QUrl());

    QMap<DeviceMode, QList<QWidget*>> m_deviceWidgets;

    QTabWidget *m_tabs = nullptr;

    template <typename T> QString EnumToString(T value)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        return QString(metaEnum.valueToKey(value));
    }

    void control();
    void setDeviceMode(DeviceMode mode);
};
#endif // MAINWINDOW_H
