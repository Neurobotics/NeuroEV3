#include "nscrollviewer.h"
#include <QScroller>
#include "ui/UICommon.h"

NScrollViewer::NScrollViewer(Qt::Orientation orientation, QWidget *parent) : QWidget(parent)
{
    UICommon::ChangeBackground(this, Qt::transparent);

    setContentsMargins(0,0,0,0);

    m_stack = new NStackPanel(orientation, this);

    m_scroll = new QScrollArea(this);
    m_scroll->setWidgetResizable(true);
    m_scroll->setWidget(m_stack);

#ifdef Q_OS_ANDROID
    QScroller::grabGesture(m_scroll, QScroller::LeftMouseButtonGesture);
#endif

    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->addWidget(m_scroll, 0, 0);
}

NScrollViewer::~NScrollViewer()
{

}

void NScrollViewer::add(QWidget *widget)
{
    addWidget(widget);
}

void NScrollViewer::remove(QWidget *widget)
{
    if (m_stack) m_stack->remove(widget);
    checkPlaceholder();
}

void NScrollViewer::setItemPadding(int uniformPadding)
{
    if (!m_stack) return;
    m_stack->setItemPadding(uniformPadding);
}

void NScrollViewer::setItemPadding(int left, int top, int right, int bottom)
{
    if (!m_stack) return;
    m_stack->setItemPadding(left, top, right, bottom);
}

QWidget *NScrollViewer::itemAt(int index)
{
    return m_stack->at(index);
}

void NScrollViewer::addWidget(QWidget *widget)
{
    m_stack->add(widget);
    checkPlaceholder();
}

void NScrollViewer::clear()
{
    if (m_stack) m_stack->clear();
    checkPlaceholder();
}

void NScrollViewer::setItemAlignment(QWidget *w, Qt::Alignment align)
{
    m_stack->setItemAlignment(w, align);
}

void NScrollViewer::setItemAlignmentByIndex(int index, Qt::Alignment align)
{
    m_stack->setItemAlignmentByIndex(index, align);
}

void NScrollViewer::setItemVisible(int index, bool visible)
{
    m_stack->at(index)->setVisible(visible);
}

QLabel *NScrollViewer::setPlaceholderText(const QString &text, Qt::Alignment alignment, Qt::Alignment textAlignment, QString objectName, int textContentsMargins)
{
    QLabel *label = new QLabel(text);
    label->setObjectName(objectName);
    label->setWordWrap(true);
    label->setContentsMargins(textContentsMargins, textContentsMargins, textContentsMargins, textContentsMargins);
    label->setAlignment(textAlignment);
    setPlaceholderWidget(label, alignment);
    return label;
}

void NScrollViewer::setPlaceholderWidget(QWidget *widget, Qt::Alignment alignment)
{
    if (m_placeholderWidget)
    {
        m_layout->removeWidget(m_placeholderWidget);
        delete m_placeholderWidget;
        m_placeholderWidget = nullptr;
    }

    if (widget)
    {
        m_placeholderWidget = widget;
        m_layout->addWidget(widget, 0, 0, 1, 1, alignment);
    }

    checkPlaceholder();
}

int NScrollViewer::count() { return m_stack->count(); }

void NScrollViewer::setTransparentBackground(bool scrollStyle)
{
    //UIHelper::ChangeBackground(scroll, Qt::transparent);
    //scroll->setStyleSheet("QScrollArea { background: transparent; border: 0; } QScrollArea > QWidget > QWidget { background: transparent; } QScrollArea > QWidget > QScrollBar { background: palette(base)}");
    setBackground(Qt::transparent, scrollStyle);
}

void NScrollViewer::setBackground(QColor color, bool scrollStyle)
{
    setBackgroundAndBorder(color, 0, Qt::black, scrollStyle);
}

void NScrollViewer::setSpacing(int spacing)
{
    if (m_stack) m_stack->setSpacing(spacing);
}

void NScrollViewer::setBackgroundAndBorder(QColor backgroundColor, quint8 borderThickness, QColor borderColor, bool scrollStyle)
{
    UICommon::ChangeBackground(m_scroll, backgroundColor);
    QString colorName = backgroundColor.name();
    if (backgroundColor == Qt::transparent) colorName = "transparent";

    QString borderStyle = "";

    if (borderThickness == 0) borderStyle = "0";
    else borderStyle = QString::number(borderThickness) +"px solid "+borderColor.name() + "";


    m_scroll->setStyleSheet("QScrollArea { background: "+colorName+"; border: "+borderStyle+"; } QScrollArea > QWidget > QWidget { background: "+colorName+"; } " +
                            (scrollStyle ? "QScrollArea > QWidget > QWidget { background: #000}" : "")
                            );
}

QScrollArea *NScrollViewer::getScrollArea()
{
    return m_scroll;
}

NStackPanel *NScrollViewer::getStackPanel()
{
    return m_stack;
}

void NScrollViewer::checkPlaceholder()
{
    if (m_placeholderWidget && m_stack) {
        m_placeholderWidget->setVisible(m_stack->count() == 0);
    }
}
