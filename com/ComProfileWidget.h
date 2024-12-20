#ifndef COMPROFILEWIDGET_H
#define COMPROFILEWIDGET_H

#include <QWidget>
#include "ComProfile.h"

class ComProfileWidget : public QWidget
{
    Q_OBJECT
public:
    ComProfileWidget(ComProfile *profile, QWidget *parent = nullptr);

protected:
    ComProfile *m_profile = nullptr;
};

#endif // COMPROFILEWIDGET_H
