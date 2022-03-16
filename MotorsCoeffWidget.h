#ifndef MOTORSCOEFFWIDGET_H
#define MOTORSCOEFFWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QDoubleSpinBox>

class MotorsCoeffWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MotorsCoeffWidget(QWidget *parent = nullptr);
    void setMotorEnabled(int motorIndex, bool enabled);
    void setMotorCoeff(int motorIndex, double coeff);

signals:
    void motorEnabledChanged(int motorIndex, bool enabled);
    void motorCoeffChanged(int motorIndex, double coeff);

protected:
    QVector<QCheckBox*> m_checkboxes;
    QVector<QDoubleSpinBox*> m_coeffs;
};

#endif // MOTORSCOEFFWIDGET_H
