#ifndef DEVICEMULTIPLAYERCONTROL_H
#define DEVICEMULTIPLAYERCONTROL_H

#include <QWidget>
#include "classes/Settings.h"
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>

class DeviceMultiplayerControl : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceMultiplayerControl(Settings *settings, QWidget *parent = nullptr);

    void setValues(QPair<float, float> meditation, QPair<float, float> concentration);

    void addWidget(QWidget *widget, Qt::Alignment align = Qt::Alignment());

protected:
    bool m_useMeditation = true;

    Settings *m_settings = nullptr;
    QVBoxLayout *m_layout = nullptr;

    QPair<float, float> m_lastMeditation;
    QPair<float, float> m_lastConcentration;

    float m_meditationMix = 0;
    float m_concentrationMix = 0;

    QList<QProgressBar*> m_meditationBars;
    QList<QProgressBar*> m_concentrationBars;

    QLabel *m_labelMix = nullptr;

    virtual void onSetValues() = 0;
    virtual void onSetControlMode() { }
};

#endif // DEVICEMULTIPLAYERCONTROL_H
