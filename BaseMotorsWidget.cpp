#include "BaseMotorsWidget.h"

BaseMotorsWidget::BaseMotorsWidget(QWidget *parent)
    : QWidget{parent}
{
    m_layout = new QVBoxLayout(this);
}

void BaseMotorsWidget::addWidgetOnTop(QWidget *widget)
{
    m_layout->insertWidget(0, widget);
}
