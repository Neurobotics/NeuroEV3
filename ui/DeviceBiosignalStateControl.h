#ifndef DEVICEBIOSIGNALSTATECONTROL_H
#define DEVICEBIOSIGNALSTATECONTROL_H

#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include "classes/Settings.h"
#include <QScrollArea>

class DeviceBiosignalStateControl : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceBiosignalStateControl(Settings *settings, QString stateEnabledPrefix = "", QWidget *parent = nullptr);

    void setCurrentState(int state);

protected:
    // QList<QVBoxLayout*> m_stateLayouts;
    Settings *m_settings = nullptr;
    QString m_stateEnabledPrefix = "";

    QList<QString> m_lastStates;
    QLabel *m_labelCurrentState = nullptr;

    void init();

    virtual void onSetCurrentState(int state) = 0;
    virtual QWidget *createStateWidget(int state, Qt::Alignment align = Qt::Alignment()) = 0;
};

#endif // DEVICEBIOSIGNALSTATECONTROL_H
