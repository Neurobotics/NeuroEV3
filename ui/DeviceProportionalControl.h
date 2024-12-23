#ifndef DEVICEPROPORTIONALCONTROL_H
#define DEVICEPROPORTIONALCONTROL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QCheckBox>
#include <QSpinBox>
#include "classes/Settings.h"

class DeviceProportionalControl : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceProportionalControl(Settings *settings, QString prefix, bool meditation, QWidget *parent = nullptr);
    void setValue(int value);

signals:
    void onUseProportional(bool proportional);
    void onSetThreshold(int value);

protected:
    bool m_isUseMeditation = true;
    bool m_useProportional = true;
    int m_threshold = 50;
    int m_value = 0;

    QVBoxLayout *m_layout = nullptr;
    QCheckBox *m_checkboxProportional = nullptr;
    QSpinBox *m_spinThreshold = nullptr;
    QProgressBar *m_bar = nullptr;

    virtual void innerSetValue() = 0;
    virtual void innerSetProportional();
    virtual void innerSetThreshold();
};

#endif // DEVICEPROPORTIONALCONTROL_H
