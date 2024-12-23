#ifndef COMCOMMANDSELECTOR_H
#define COMCOMMANDSELECTOR_H

#include <QComboBox>
#include "com/ComDevice.h"

class ComCommandSelector : public QComboBox
{
    Q_OBJECT
public:
    explicit ComCommandSelector(ComDevice *com, QString action, QWidget *parent = nullptr);

signals:
};

#endif // COMCOMMANDSELECTOR_H
