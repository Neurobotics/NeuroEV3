#include "LabelWithIcon.h"

LabelWithIcon::LabelWithIcon(QIcon icon, QString text, QString tooltip, QSize iconSize, QWidget *parent)
    : QPushButton{parent}
{
    setText(text);
    setIcon(icon);
    setIconSize(iconSize);
    setToolTip(tooltip);
    setLayoutDirection(Qt::RightToLeft);
    setFlat(true);
    setStyleSheet("QPushButton { padding: 0 }");
}
