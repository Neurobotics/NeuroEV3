#ifndef ICONLABEL_H
#define ICONLABEL_H

#include <QWidget>
#include <QLabel>

class IconLabel : public QWidget
{
    Q_OBJECT
public:
    explicit IconLabel(QIcon inactive, QIcon active, QString text = "", QSize size = QSize(24, 24), QWidget *parent = nullptr);
    void setText(QString text);
    void setActive(bool active);
    void setLabelStyle(QString style);

protected:
    bool m_active = false;

    QLabel *m_label = nullptr;

    QPixmap m_pixmapActive;
    QPixmap m_pixmapInactive;

    void paintEvent(QPaintEvent *);
};

#endif // ICONLABEL_H
