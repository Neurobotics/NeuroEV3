#ifndef NSTACKPANEL_H
#define NSTACKPANEL_H

#include <QWidget>
#include <QGridLayout>

class NStackPanel : public QWidget
{
Q_OBJECT
public:
    explicit NStackPanel(Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr);

    ~NStackPanel();

    void insert(int index, QWidget *w);
    void add(QWidget* w);
    void remove(QWidget *w);
    void setSpacing(int spacing);
    void setPadding(int padding);
    void setPadding(int left, int top, int right, int bottom);

    void setItemPadding(int padding);
    void setItemPadding(int left, int top, int right, int bottom);

    void setItemAlignment(QWidget *w, Qt::Alignment align);
    void setItemAlignmentByIndex(int index, Qt::Alignment align);

    void setOrientation(Qt::Orientation orientation);

    NStackPanel* operator << (QWidget* w);
    void clear();
    QWidget *at(int index);
    int count();

    QBoxLayout::Direction boxOrientation();

private:
    Qt::Orientation m_orientation;
    QBoxLayout *m_lay = nullptr;
    QBoxLayout *m_layMain = nullptr;
    QList<QWidget*> m_widgets;
    QMargins m_padding = QMargins(0,0,0,0);
};

#endif // NSTACKPANEL_H
