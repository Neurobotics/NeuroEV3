#include "ComDeviceManualControl.h"
#include <QPushButton>
#include <QGridLayout>
#include <QCoreApplication>
#include <QLabel>
#include "ui/ImpovedSlider.h"

ComDeviceManualControl::ComDeviceManualControl(ComDevice *com, QWidget *parent) : QWidget(parent)
{
    auto speedSlider = new ImprovedSlider(Qt::Horizontal);
    speedSlider->setRange(-10, 10);
    speedSlider->setFixedWidth(208);
    speedSlider->setTickInterval(2);
    speedSlider->setTickPosition(QSlider::TickPosition::TicksAbove);
    speedSlider->setSingleStep(1);
    speedSlider->setPageStep(1);
    connect(speedSlider, QOverload<int>::of(&QSlider::valueChanged), [=](int value) {
        m_com->sendSpeed(value * 10);
    });

    auto func_btn = [=](QString command, QString text, QString style = "") {
        auto btn = new QPushButton(text);
        btn->setFixedSize(60, 60);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setToolTip(QCoreApplication::translate("Generic", qPrintable(command)));
        if (!style.isEmpty()) {
            btn->setStyleSheet(style);
        }
        connect(btn, &QPushButton::clicked, [=]() {
            speedSlider->setValue(0);
            if (m_com) {
                m_com->sendCommand(command);
            }            
        });
        return btn;
    };

    auto central = new QWidget();
    auto grid = new QGridLayout(central);
    grid->addWidget(func_btn(ComDevice::FORWARD, "↑", "QPushButton { font-size: 24px }"), 0, 1);
    grid->addWidget(func_btn(ComDevice::TURNLEFT, "←", "QPushButton { font-size: 24px }"), 1, 0);
    grid->addWidget(func_btn(ComDevice::STOP, "X", "QPushButton { font-size: 24px; color: #C30 }"), 1, 1);
    grid->addWidget(func_btn(ComDevice::TURNRIGHT, "→", "QPushButton { font-size: 24px }"), 1, 2);
    grid->addWidget(func_btn(ComDevice::BACKWARDS, "↓", "QPushButton { font-size: 24px }"), 2, 1);

    grid->addWidget(new QLabel(" "), grid->rowCount(), 0);

    int r = grid->rowCount();
    for (int i = 0; i<3; i++) {
        grid->addWidget(func_btn("Custom" + QString::number(i+1), QCoreApplication::translate("Generic", "Custom") + " \n" + QString::number(i+1)), r, i);
    }

    grid->addWidget(new QLabel(" "), grid->rowCount(), 0);
    grid->addWidget(speedSlider, grid->rowCount(), 0, 1, grid->columnCount());

    auto lay = new QVBoxLayout(this);
    lay->addWidget(central, 0, Qt::AlignCenter);

    m_com = com;
}
