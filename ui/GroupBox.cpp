#include "GroupBox.h"
#include "UICommon.h"

GroupBox::GroupBox(QString title, bool linear, QWidget *parent) : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    setObjectName("gb");

    m_check = new QCheckBox(title);
    connect(m_check, &QCheckBox::toggled, [=](bool on) {
        emit toggled(on);
    });

    m_linear = linear;

    m_headerLayout = new QHBoxLayout();
    m_headerLayout->setContentsMargins(0,0,0,0);
    m_headerLayout->addWidget(m_check);
    m_headerLayout->setSpacing(0);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setContentsMargins(0, 0, 0, 0);

    layout->addLayout(m_headerLayout);
    layout->addLayout(m_contentLayout);

    setContentsMargins(0,0,0,0);
}

bool GroupBox::isChecked()
{
    return m_check->isChecked();
}

void GroupBox::setChecked(bool checked)
{
    m_check->setChecked(checked);
}

void GroupBox::addHeaderWidget(QWidget *w)
{
    m_headerLayout->addWidget(w);
}

void GroupBox::addWidget(QWidget *w)
{
    if (m_linear) {
        m_headerLayout->addWidget(w, 0, Qt::AlignLeft);
    } else {
        m_contentLayout->addWidget(w);
    }
}

void GroupBox::addHeaderSpacing(int spacing)
{
    m_headerLayout->addSpacing(spacing);
}
