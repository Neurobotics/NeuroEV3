#ifndef IMPOVEDSLIDER_H
#define IMPOVEDSLIDER_H

#include <QSlider>

class ImprovedSlider : public QSlider
{
    Q_OBJECT
public:
    explicit ImprovedSlider(Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr);

signals:
    void onClick(int value);

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // IMPOVEDSLIDER_H
