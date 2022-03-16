#ifndef MOTORSCOEFFWIDGET_H
#define MOTORSCOEFFWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>

class MotorsCoeffWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MotorsCoeffWidget(QWidget *parent = nullptr);
    void setMotorEnabled(int motorIndex, bool enabled);
    void setMotorCoeff(int motorIndex, double coeff);

    void addWidgetOnTop(QWidget *widget);

signals:
    void motorEnabledChanged(int motorIndex, bool enabled);
    void motorCoeffChanged(int motorIndex, double coeff);

protected:
    QVector<QCheckBox*> m_checkboxes;
    QVector<QDoubleSpinBox*> m_coeffs;
    QVBoxLayout *m_layout = nullptr;
};

#endif // MOTORSCOEFFWIDGET_H
