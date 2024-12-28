#include "BiosignalStateCircle.h"
#include <QPainter>
#include "ui/UICommon.h"
#include <QTimer>

BiosignalStateCircle::BiosignalStateCircle(int state, bool filled, QSize size, QWidget *parent) : QWidget(parent)
{
    setFixedSize(size);
    setStateAndFilled(state, filled);
    QTimer::singleShot(500, [=]() {
        update();
    });
}

void BiosignalStateCircle::setState(int state)
{
    setStateAndFilled(state, m_filled);
}

void BiosignalStateCircle::setFilled(bool filled)
{
    setStateAndFilled(m_state, filled);
}

void BiosignalStateCircle::setStateAndFilled(int state, bool filled)
{
    m_state = state;
    m_stateStr = QString::number(state);
    m_filled = filled;
    update();
}

void BiosignalStateCircle::setSquare(bool on)
{
    m_square = on;
    update();
}

int BiosignalStateCircle::state()
{
    return m_state;
}

void BiosignalStateCircle::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::transparent);

    // if (m_state == 0) return;

    int w = width();
    int h = height();
    int s = qMin(w, h) - 4;

    if (s < 4) return;

    p.translate((w-s)/2, (h-s)/2);

    p.setRenderHint(QPainter::Antialiasing);

    QColor color = UICommon::Instance()->biosignalStateColor(m_state);

    if (m_filled) {
        p.setPen(Qt::NoPen);
        p.setBrush(color);
    } else {
        QPen pen(color);
        pen.setWidth(qMax(s / 10, 2));
        p.setPen(pen);
        p.setBrush(color.lighter(200));
    }

    QRect r(0, 0, s, s);
    if (m_square) {
        p.drawRect(r);
    } else {
        p.drawEllipse(r);
    }

    if (m_state <= 0) return;
    auto font = p.font();
    font.setPixelSize(s * 0.75);
    font.setBold(true);
    p.setFont(font);
    p.translate(0, -qMax(1, s / 12));
    p.setPen(Qt::black);

    p.drawText(r, m_stateStr, QTextOption(Qt::AlignCenter));
}
