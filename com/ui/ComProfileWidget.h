#ifndef COMPROFILEWIDGET_H
#define COMPROFILEWIDGET_H

#include <QWidget>
#include "com/ComDevice.h"

class ComProfileWidget : public QWidget
{
    Q_OBJECT
public:
    ComProfileWidget(ComDevice *com, QWidget *parent = nullptr);

protected:
    ComProfile *m_profile = nullptr;
};

#endif // COMPROFILEWIDGET_H
