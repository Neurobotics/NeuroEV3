#include "ComDeviceStatusWidget.h"

ComDeviceStatusWidget::ComDeviceStatusWidget(ComDevice *com, QWidget *parent) : IconLabel(QIcon(":/resources/disconnected.svg"), QIcon(":/resources/connected.svg"), "", QSize(40, 40))
{
    if (com) {
        connect(com, &ComDevice::connected, [=]() {
            setActive(true);
        });

        connect(com, &ComDevice::disconnected, [=]() {
            setActive(false);
        });

        setActive(com->isConnected());

        setText(QString(com->profile()->getPort()).replace("COM", "COM "));
    }

    setLabelStyle("font-size: 10px; line-height: 10px");
}
