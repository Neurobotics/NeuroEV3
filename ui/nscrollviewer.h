#ifndef NSCROLLVIEWER_H
#define NSCROLLVIEWER_H

#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include "nstackpanel.h"
#include <QLabel>

class NScrollViewer : public QWidget
{
Q_OBJECT
public:
    explicit NScrollViewer(Qt::Orientation orientation = Qt::Vertical, QWidget *parent = nullptr);

    ~NScrollViewer();

    void addWidget(QWidget* widget);
    void add(QWidget* widget);
    void insertWidget(int index, QWidget *widget);
    void remove(QWidget *widget);

    void setItemPadding(int uniformPadding);
    void setItemPadding(int left, int top, int right, int bottom);

    QWidget *itemAt(int index);

    void clear();
    void setItemAlignment(QWidget *w, Qt::Alignment align);
    void setItemAlignmentByIndex(int index, Qt::Alignment align);
    void setItemVisible(int index, bool visible);

    QLabel *setPlaceholderText(const QString &text, Qt::Alignment alignment = Qt::Alignment(), Qt::Alignment textAlignment = Qt::AlignCenter, QString objectName = "NScrollViewerPlaceholder", int textContentsMargins = 16);
    void setPlaceholderWidget(QWidget *widget, Qt::Alignment alignment = Qt::Alignment());

    int count();

    void setTransparentBackground(bool scrollStyle = false);

    void setBackground(QColor color, bool scrollStyle = false);

    void setSpacing(int spacing);

    void setBackgroundAndBorder(QColor backgroundColor, quint8 borderThickness, QColor borderColor, bool scrollStyle = true);

    QScrollArea *getScrollArea();
    NStackPanel *getStackPanel();

private:
    QScrollArea *m_scroll = nullptr;
    QGridLayout *layout = nullptr;
    NStackPanel *m_stack = nullptr;
    QWidget *m_placeholderWidget = nullptr;
    QGridLayout *m_layout = nullptr;

public slots:
    void checkPlaceholder();
};

#endif // NSCROLLVIEWER_H
