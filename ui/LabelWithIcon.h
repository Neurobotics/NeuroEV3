#ifndef LABELWITHICON_H
#define LABELWITHICON_H

#include <QPushButton>

class LabelWithIcon : public QPushButton
{
    Q_OBJECT
public:
    explicit LabelWithIcon(QIcon icon, QString text, QString tooltip, QSize iconSize = QSize(16,16), QWidget *parent = nullptr);

};

#endif // LABELWITHICON_H
