#include "ComDeviceControl.h"
#include <QPushButton>
#include <QGridLayout>
#include <QCoreApplication>
#include <QLabel>

ComDeviceControl::ComDeviceControl(QWidget *parent) : QWidget(parent)
{
    auto func_btn = [=](QString command, QString text) {
        auto btn = new QPushButton(text);
        btn->setMinimumSize(40, 40);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setToolTip(QCoreApplication::translate("Generic", qPrintable(command)));
        connect(btn, &QPushButton::clicked, [=]() {
            if (m_com) {
                m_com->sendCommand(command);
            }
        });
        return btn;
    };

    auto grid = new QGridLayout();
    grid->addWidget(func_btn("Forward", "↑"), 0, 1);
    grid->addWidget(func_btn("TurnLeft", "←"), 1, 0);
    grid->addWidget(func_btn("Stop", "X"), 1, 1);
    grid->addWidget(func_btn("TurnLeft", "→"), 1, 2);
    grid->addWidget(func_btn("Backwards", "↓"), 2, 1);

    grid->addWidget(new QLabel(" "), 3, 0);

    for (int i = 0; i<3; i++) {
        grid->addWidget(func_btn("Custom" + QString::number(i+1), "Custom \n" + QString::number(i+1)), 4, i);
    }

    setEnabled(false);
}

void ComDeviceControl::setComDevice(ComDevice *com)
{
    m_com = com;

    setEnabled(com != nullptr);
}
