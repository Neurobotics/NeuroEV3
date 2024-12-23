#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMetaEnum>
#include <QPushButton>
#include <QMainWindow>
#include <QGridLayout>
#include <QTabWidget>

#include "ev3/EV3.h"
#include "com/ComDevice.h"

#include "classes/Settings.h"
#include "classes/NeuroPlayAppConnector.h"

#include "ui/DeviceProportionalControl.h"
#include "ui/DeviceMultiplayerControl.h"
#include "ui/DeviceBiosignalStateControl.h"

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
        None,
        Manual,
        MentalState,
        Meditation,
        Concentration,
        Multiplayer
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

    ComDevice *m_com = nullptr;

    QList<UserBCI> m_userBCI;

    ControlState m_controlState = Manual;
    bool m_canControl = true;

    Settings *m_settings = nullptr;

    NeuroPlayAppConnector *m_neuroplayConnector = nullptr;

    QWidget *newVersionButton();
    QPushButton *flatButton(QString text, QIcon icon = QIcon(), QString tooltip = "", QUrl url = QUrl());

    QMap<DeviceMode, QList<QWidget*>> m_deviceWidgets;

    QTabWidget *m_tabs = nullptr;

    void control();
    void setDeviceMode(DeviceMode mode);

    void addTab(QString title, QWidget *widgetForEV3, QWidget *widgetForCOM);
    void addTab(QString title, DeviceProportionalControl *widgetForEV3, DeviceProportionalControl *widgetForCOM, bool isMeditation);

    QList<DeviceProportionalControl*> m_meditationControls;
    QList<DeviceProportionalControl*> m_concentrationControls;
    QList<DeviceMultiplayerControl*> m_multiplayerControls;
    QList<DeviceBiosignalStateControl*> m_biosignalStateControls;
};
#endif // MAINWINDOW_H
