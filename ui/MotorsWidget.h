#ifndef MOTORSWIDGET_H
#define MOTORSWIDGET_H

#include "BaseMotorsWidget.h"
#include <QSlider>

class QImprovedSlider : public QSlider
{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *event);

public:
    explicit QImprovedSlider(QWidget *parent = 0);

signals:
    void onClick(int value);
};

class MotorsWidget : public BaseMotorsWidget
{
    Q_OBJECT
public:
    explicit MotorsWidget(QWidget *parent = nullptr);
    void setMotorValue(int motorIndex, int value);

signals:
    void motorValueChangeRequest(int motor, int value);

protected:
    QVector<QSlider*> m_sliders;
};

#endif // MOTORSWIDGET_H
