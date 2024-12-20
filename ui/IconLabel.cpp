#include "IconLabel.h"
#include <QGridLayout>
#include <QPainter>
#include <QPixmap>
#include <QIcon>

IconLabel::IconLabel(QIcon inactive, QIcon active, QString text, QSize size, QWidget *parent) : QWidget{parent}
{
    if (!active.isNull()) m_pixmapActive = active.pixmap(size);
    if (!inactive.isNull()) m_pixmapInactive = inactive.pixmap(size);

    m_label = new QLabel(text);
    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignCenter);

    auto grid = new QGridLayout(this);
    grid->setContentsMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
    grid->addWidget(m_label, 0, 0, 1, 1, Qt::AlignCenter);
    setFixedSize(size);
}

void IconLabel::setText(QString text)
{
    m_label->setText(text);
}

void IconLabel::setActive(bool active)
{
    if (m_active == active) return;
    m_active = active;
    update();
}

void IconLabel::setLabelStyle(QString style)
{
    m_label->setStyleSheet(style);
}

void IconLabel::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(0, 0, m_active ? m_pixmapActive : m_pixmapInactive);
    p.end();
}
