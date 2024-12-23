#include "ComCommandSelector.h"

ComCommandSelector::ComCommandSelector(ComDevice *com, QString action, QWidget *parent)
    : QComboBox(parent)
{
    QString selectedAction = com->action(action);

    addItem("-");

    foreach (auto command, ComDevice::Commands()) {
        addItem(command.title, command.key);
        if (command.key == selectedAction) {
            setCurrentIndex(count() - 1);
        }
    }

    connect(this, &QComboBox::currentIndexChanged, [=]() {
        com->setAction(action, currentData().toString());
    });

    setMinimumWidth(150);
}
