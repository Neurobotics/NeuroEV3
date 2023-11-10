#include "MotorsWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "IconLabel.h"
#include "classes/Common.h"

MotorsWidget::MotorsWidget(QWidget *parent) : BaseMotorsWidget(parent)
{
    m_layout->setSpacing(2);
    for (int i = 1; i<=MAX_MOTORS; i++) {
        auto motorPowerLabel = new QLabel("0");
        motorPowerLabel->setMinimumWidth(50);

        auto motorSocket = Common::Instance()->motorSocket(i);

        auto motorPower = new QImprovedSlider();
        motorPower->setRange(-100, 100);
        motorPower->setFixedWidth(208);
        motorPower->setTickInterval(100);
        motorPower->setTickPosition(QSlider::TickPosition::TicksAbove);
        motorPower->setValue(0);
        motorPower->setOrientation(Qt::Horizontal);
        connect(motorPower, QOverload<int>::of(&QSlider::valueChanged), this, [=](int val) {
            motorPowerLabel->setText(QString::number(val));
            emit motorValueChangeRequest(i, val);
            motorSocket->setActive(val != 0);
        }, Qt::QueuedConnection);

        auto btnZero = new QPushButton();
        btnZero->setIcon(QIcon(":/resources/zero.svg"));
        btnZero->setCursor(Qt::PointingHandCursor);
        btnZero->setToolTip(tr("Set to zero"));
        btnZero->setFixedSize(24, 24);
        connect(btnZero, &QPushButton::clicked, [=]() {
            motorPower->setValue(0);
        });

        m_sliders << motorPower;

        auto motorLayout = new QHBoxLayout();
        motorLayout->setSpacing(4);

        motorLayout->addWidget(motorSocket);
        motorLayout->addWidget(motorPower);
        motorLayout->addWidget(motorPowerLabel);
        motorLayout->addWidget(btnZero, 100, Qt::AlignLeft);

        m_layout->addLayout(motorLayout);
    }
    m_layout->addWidget(new QLabel(), 100);
}

void MotorsWidget::setMotorValue(int motorIndex, int value)
{
    motorIndex --;
    if (motorIndex >= 0 && motorIndex < m_sliders.length()) {
        m_sliders[motorIndex]->setValue(value);
    }
}

#include <QStyleOptionSlider>
#include <QSlider>
#include <QMouseEvent>
void QImprovedSlider::mousePressEvent(QMouseEvent *event)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    if (event->button() == Qt::LeftButton &&
        !sr.contains(event->pos())) {
        int newVal;
        if (orientation() == Qt::Vertical) {
            double halfHandleHeight = (0.5 * sr.height()) + 0.5;
            int adaptedPosY = height() - event->y();
            if ( adaptedPosY < halfHandleHeight )
                adaptedPosY = halfHandleHeight;
            if ( adaptedPosY > height() - halfHandleHeight )
                adaptedPosY = height() - halfHandleHeight;
            double newHeight = (height() - halfHandleHeight) - halfHandleHeight;
            double normalizedPosition = (adaptedPosY - halfHandleHeight)  / newHeight ;

            newVal = minimum() + (maximum()-minimum()) * normalizedPosition;
        } else {
            double halfHandleWidth = (0.5 * sr.width()) + 0.5;
            int adaptedPosX = event->x();
            if ( adaptedPosX < halfHandleWidth )
                adaptedPosX = halfHandleWidth;
            if ( adaptedPosX > width() - halfHandleWidth )
                adaptedPosX = width() - halfHandleWidth;
            double newWidth = (width() - halfHandleWidth) - halfHandleWidth;
            double normalizedPosition = (adaptedPosX - halfHandleWidth)  / newWidth ;

            newVal = minimum() + ((maximum()-minimum()) * normalizedPosition);
        }

        if (invertedAppearance())
            this->setValue(maximum() - newVal);
        else
            this->setValue(newVal);

        event->accept();
    }
    else {
        QSlider::mousePressEvent(event);
    }
    emit onClick(this->value());
}

QImprovedSlider::QImprovedSlider(QWidget *parent)
{

}
