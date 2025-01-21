#ifndef DEVICEBIOSIGNALSTATECONTROL_H
#define DEVICEBIOSIGNALSTATECONTROL_H

#include <QWidget>
#include <QVBoxLayout>
#include "classes/Settings.h"
#include <QScrollArea>
#include "ui/BiosignalStateCircle.h"
#include <QVBoxLayout>
#include "ui/nscrollviewer.h"

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

    void rebuildStateControls();

    QList<BiosignalStateCircle*> m_controlStateCirles;
    QList<BiosignalStateCircle*> m_lastControlStateCirles;

    QList<int> m_controlAccum;
    int m_controlIndex = 0;
    int m_controlState = 0;

    virtual void onSetCurrentState(int state) = 0;
    virtual QWidget *createStateWidget(int state) = 0;

    NScrollViewer *m_scroll = nullptr;

    QList<int> statesForWord(int maxStates, int wordLength);

    QVBoxLayout *m_layout = nullptr;
};

#endif // DEVICEBIOSIGNALSTATECONTROL_H
