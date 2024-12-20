#include "ComDeviceControl.h"
#include <QPushButton>
#include <QGridLayout>
#include <QCoreApplication>
#include <QLabel>

ComDeviceControl::ComDeviceControl(QWidget *parent) : QWidget(parent)
{
    auto func_btn = [=](QString command, QString text, QString style = "") {
        auto btn = new QPushButton(text);
        btn->setFixedSize(60, 60);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setToolTip(QCoreApplication::translate("Generic", qPrintable(command)));
        if (!style.isEmpty()) {
            btn->setStyleSheet(style);
        }
        connect(btn, &QPushButton::clicked, [=]() {
            if (m_com) {
                m_com->sendCommand(command);
            }
        });
        return btn;
    };

    auto central = new QWidget();
    auto grid = new QGridLayout(central);
    grid->addWidget(func_btn("Forward", "↑", "QPushButton { font-size: 24px }"), 0, 1);
    grid->addWidget(func_btn("TurnLeft", "←", "QPushButton { font-size: 24px }"), 1, 0);
    grid->addWidget(func_btn("Stop", "X", "QPushButton { font-size: 24px; color: #C30 }"), 1, 1);
    grid->addWidget(func_btn("TurnRight", "→", "QPushButton { font-size: 24px }"), 1, 2);
    grid->addWidget(func_btn("Backwards", "↓", "QPushButton { font-size: 24px }"), 2, 1);

    grid->addWidget(new QLabel(" "), 3, 0);

    for (int i = 0; i<3; i++) {
        grid->addWidget(func_btn("Custom" + QString::number(i+1), "Custom \n" + QString::number(i+1)), 4, i);
    }

    setEnabled(false);

    auto lay = new QVBoxLayout(this);
    lay->addWidget(central, 0, Qt::AlignCenter);
}

void ComDeviceControl::setComDevice(ComDevice *com)
{
    m_com = com;
    setEnabled(com != nullptr);
}
