#ifndef BASEMOTORSWIDGET_H
#define BASEMOTORSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

class BaseMotorsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BaseMotorsWidget(QWidget *parent = nullptr);

    void addWidgetOnTop(QWidget *widget);

protected:
    QVBoxLayout *m_layout = nullptr;
};

#endif // BASEMOTORSWIDGET_H
