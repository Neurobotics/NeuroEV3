#ifndef DEVICEBIOSIGNALSTATECONTROL_H
#define DEVICEBIOSIGNALSTATECONTROL_H

#include <QWidget>
#include <QVBoxLayout>
#include "classes/Settings.h"
#include <QScrollArea>
#include "ui/BiosignalStateCircle.h"

class DeviceBiosignalStateControl : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceBiosignalStateControl(Settings *settings, QString stateEnabledPrefix = "", QWidget *parent = nullptr);

    void setCurrentState(int state);

protected:
    Settings *m_settings = nullptr;
    QString m_stateEnabledPrefix = "";

    QList<int> m_lastStates;

    int m_maxDisplayedStates = 16;

    QList<BiosignalStateCircle*> m_stateCircles;

    void init();

    virtual void onSetCurrentState(int state) = 0;
    virtual QWidget *createStateWidget(int state, Qt::Alignment align = Qt::Alignment()) = 0;
};

#endif // DEVICEBIOSIGNALSTATECONTROL_H
