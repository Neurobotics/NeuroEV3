#include "nstackpanel.h"
#include <QGridLayout>

NStackPanel::NStackPanel(Qt::Orientation orientation, QWidget *parent) : QWidget(parent)
{
    m_orientation = orientation;

    auto orient = boxOrientation();

    m_layMain = new QBoxLayout(orient, this);
    m_layMain->setContentsMargins(0,0,0,0);

    m_lay = new QBoxLayout(orient);
    m_lay->setContentsMargins(0,0,0,0);

    m_layMain->addLayout(m_lay, 0);
    m_layMain->addStretch(100);

    setContentsMargins(0,0,0,0);
}

NStackPanel::~NStackPanel()
{

}

void NStackPanel::add(QWidget *w)
{
    if (w == nullptr) return;
    m_lay->addWidget(w);
    m_widgets << w;
    // realign();
}

void NStackPanel::remove(QWidget *w)
{
    int index = m_widgets.indexOf(w);
    if (index >= 0)
    {
        m_widgets[index]->setParent(nullptr);
        m_widgets.removeAt(index);
        // realign();
    }
}

void NStackPanel::setSpacing(int spacing)
{
    if (m_lay) m_lay->setSpacing(spacing);
}


void NStackPanel::setPadding(int padding)
{
    setPadding(padding, padding, padding, padding);
}

void NStackPanel::clear()
{
    foreach (auto w, m_widgets) {
        w->hide();
        w->setParent(nullptr);
        w->deleteLater();
    }
    m_widgets.clear();
}

QWidget *NStackPanel::at(int index)
{
    if (index >= 0 && index < count())
        return m_widgets[index];

    return nullptr;
}

int NStackPanel::count() { return m_widgets.count(); }

QBoxLayout::Direction NStackPanel::boxOrientation()
{
    return m_orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;
}

void NStackPanel::setPadding(int left, int top, int right, int bottom)
{
    m_padding = QMargins(left, top, right, bottom);
    m_lay->setContentsMargins(m_padding);
}

void NStackPanel::setItemPadding(int margin)
{
    setItemPadding(margin, margin, margin, margin);
}

void NStackPanel::setItemPadding(int left, int top, int right, int bottom)
{
    foreach (auto w, m_widgets)
    {
        w->setContentsMargins(left, top, right, bottom);
    }
}

void NStackPanel::setItemAlignment(QWidget *w, Qt::Alignment align)
{
    if (w) m_lay->setAlignment(w, align);
}

void NStackPanel::setItemAlignmentByIndex(int index, Qt::Alignment align)
{
    if (index >= 0 && index < count())
    {
        setItemAlignment(at(index), align);
    }
}

void NStackPanel::setOrientation(Qt::Orientation orientation)
{
    m_orientation = orientation;

    auto orient = boxOrientation();

    m_layMain->setDirection(orient);
    m_lay->setDirection(orient);
}

NStackPanel *NStackPanel::operator <<(QWidget *w)
{
    add(w);
    return this;
}
