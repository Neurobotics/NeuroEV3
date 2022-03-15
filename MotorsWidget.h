#ifndef MOTORSWIDGET_H
#define MOTORSWIDGET_H

#include <QWidget>
#include <QSlider>

class MotorsWidget : public QWidget
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
