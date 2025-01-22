#include "SequencePlayer.h"
#include <QPushButton>
#include <QCoreApplication>
#include <QMenu>
#include "ui/LabelWithIcon.h"

SequencePlayer::SequencePlayer(QWidget *parent) : QWidget(parent)
{
    m_btnPlay = squareButton(QIcon(":/resources/play.svg"), "", QCoreApplication::translate("Generic", "Play sequence"));
    m_btnPlay->setCursor(Qt::PointingHandCursor);
    connect(m_btnPlay, &QPushButton::clicked, [=]() { play(); });

    m_btnStop = squareButton(QIcon(":/resources/stop.svg"), "", QCoreApplication::translate("Generic", "Stop sequence"));
    m_btnStop->setCursor(Qt::PointingHandCursor);
    connect(m_btnStop, &QPushButton::clicked, [=]() { stop(); });

    m_btnCheck = new QPushButton(QIcon(":/resources/sequence.svg"), "Режим последовательности");
    m_btnCheck->setCheckable(true);
    connect(m_btnCheck, &QPushButton::toggled, [=](bool on) {
        enableSequence(on);
    });

    m_spinTimeStep = new QDoubleSpinBox();
    m_spinTimeStep->setRange(0.1, 10);
    m_spinTimeStep->setSingleStep(0.1);
    m_spinTimeStep->setValue(1000);
    m_spinTimeStep->setSuffix(QCoreApplication::translate("Generic", "s"));
    m_spinTimeStep->setDecimals(1);
    connect(m_spinTimeStep, &QDoubleSpinBox::valueChanged, [=](double value) {
        setInterval(value * 1000);
        emit intervalChanged(m_intervalMs);
    });

    auto btnClear = new QPushButton(QIcon(":/resources/delete.svg"), "");
    btnClear->setFixedSize(QSize(24, 24));
    btnClear->setIconSize(QSize(16, 16));
    btnClear->setToolTip(QCoreApplication::translate("Generic", "Clear sequence"));
    connect(btnClear, &QPushButton::clicked, [=]() {
        clearCommands();
    });

    auto iconTimer = new LabelWithIcon(QIcon(":/resources/timer.svg"), "", QCoreApplication::translate("Generic", "Time step"));

    auto timerLayout = new QHBoxLayout();
    timerLayout->setContentsMargins(0,0,0,0);
    timerLayout->addWidget(iconTimer);
    timerLayout->addWidget(m_spinTimeStep, 100);

    auto lay = new QHBoxLayout();
    lay->addWidget(m_btnPlay);
    lay->addWidget(m_btnStop);
    lay->addWidget(btnClear, 100, Qt::AlignTop|Qt::AlignRight);

    auto controlsLayout = new QVBoxLayout();
    controlsLayout->addLayout(lay);
    controlsLayout->addStretch(1);
    controlsLayout->addLayout(timerLayout);

    m_sequenceWidget = new QWidget();
    m_sequenceWidget->setContentsMargins(0,0,0,0);
    auto sequenceLayout = new QHBoxLayout(m_sequenceWidget);
    sequenceLayout->setContentsMargins(0,0,0,0);
    m_commandsScroll = new NScrollViewer(Qt::Horizontal);
    m_commandsScroll->setMinimumSize(10,10);
    m_commandsScroll->setFixedHeight(64);
    sequenceLayout->addWidget(m_commandsScroll, 100);
    sequenceLayout->addLayout(controlsLayout, 0);

    auto vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0,0,0,0);
    vbox->addWidget(m_btnCheck, 0, Qt::AlignLeft);
    vbox->addWidget(m_sequenceWidget);

    enableSequence(false);
    clearCommands();

    m_commandsScroll->setStyleSheet(m_btnStyle + "}");

    m_commandsScroll->setSpacing(10);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [=]() {
        nextCommand();
    });
}

void SequencePlayer::addCommand(QString cmd, QString view)
{
    if (m_isPlaying) return;

    m_commands << cmd;
    auto btn = squareButton(QIcon(), view, cmd);
    btn->setObjectName("active");
    btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(btn, &QWidget::customContextMenuRequested, [=]() {
        QMenu menu;
        int index = m_commandsWidgets.indexOf(btn);
        int n = m_commands.length();
        menu.addAction(QIcon(":/resources/play.svg"), QCoreApplication::translate("Generic", "Play from this command"), [=]() {
            if (index >= 0 && index < n) {
                playFrom(index, true);
            }
        });

        menu.addSeparator();
        auto func_move = [=](int index, int inc) {
            auto newIndex = index + inc;
            if (newIndex >= 0 && newIndex < n) {
                m_commands.swapItemsAt(index, newIndex);

                auto btn1 = m_commandsWidgets.at(index);
                auto btn2 = m_commandsWidgets.at(newIndex);

                btn1->setParent(nullptr);
                btn2->setParent(nullptr);

                auto ind = qMin(index, newIndex);
                if (newIndex > index) {
                    m_commandsScroll->insertWidget(ind, btn1);
                    m_commandsScroll->insertWidget(ind, btn2);
                } else {
                    m_commandsScroll->insertWidget(ind, btn2);
                    m_commandsScroll->insertWidget(ind, btn1);
                }
                m_commandsWidgets.swapItemsAt(index, newIndex);
            }
        };
        auto actionLeft = menu.addAction(QCoreApplication::translate("Generic", "Move 1 position to the left"), [=]() {
            func_move(index, -1);
        });
        auto actionRight = menu.addAction(QCoreApplication::translate("Generic", "Move 1 position to the right"), [=]() {
            func_move(index, 1);
        });
        if (index == 0) actionLeft->setDisabled(true);
        if (index + 1 == n) actionRight->setDisabled(true);

        menu.addSeparator();
        menu.addAction(QIcon(":/resources/delete.svg"), QCoreApplication::translate("Generic", "Remove"), [=]() {
            if (index >= 0 && index < n) {
                m_commands.removeAt(index);
                btn->setVisible(false);
                btn->setParent(nullptr);
                m_commandsWidgets.removeAt(index);
            }
        });

        menu.addSeparator();
        menu.addAction(QIcon(":/resources/delete.svg"), QCoreApplication::translate("Generic", "Clear sequence"), [=]() {
            clearCommands();
        });

        menu.exec(QCursor::pos());
    });
    m_commandsScroll->add(btn);
    m_commandsWidgets << btn;
}

void SequencePlayer::enableSequence(bool on)
{
    stop();
    m_sequenceWidget->setVisible(on);

    m_btnCheck->blockSignals(true);
    m_btnCheck->setChecked(on);
    m_btnCheck->blockSignals(false);
}

void SequencePlayer::clearCommands()
{
    stop();
    m_commands.clear();
    m_commandsScroll->clear();
    m_commandsWidgets.clear();
}

void SequencePlayer::play(bool on)
{
    playFrom(0, on);
}

void SequencePlayer::playFrom(int index, bool on)
{
    if (m_timer) {
        m_timer->stop();
    }

    m_btnPlay->setVisible(!on);
    m_btnStop->setVisible(on);

    m_commandIndex = index;

    if (m_timer && on) {
        m_timer->setInterval(m_intervalMs);
        m_timer->start();
        nextCommand();
    }
}

void SequencePlayer::stop()
{
    play(false);
}

bool SequencePlayer::sequenceEnabled()
{
    return m_btnCheck->isChecked();
}

void SequencePlayer::setInterval(int ms)
{
    m_intervalMs = qBound(10, ms, 10000);
    m_spinTimeStep->blockSignals(true);
    m_spinTimeStep->setValue(m_intervalMs / 1000.0);
    m_spinTimeStep->blockSignals(false);
}

QPushButton *SequencePlayer::squareButton(QIcon icon, QString text, QString tooltip)
{
    auto btn = new QPushButton(text);
    btn->setIcon(icon);
    btn->setIconSize(QSize(24, 24));
    btn->setCursor(Qt::PointingHandCursor);
    btn->setToolTip(tooltip);
    btn->setFixedSize(32, 32);
    btn->setStyleSheet(m_btnStyle + "}");
    btn->setContentsMargins(0,0,0,0);
    return btn;
}

void SequencePlayer::nextCommand()
{
    // qDebug() << "T" << m_commandIndex << m_commands.length();
    if (m_commandIndex >= 0) {
        if (m_commandIndex < m_commands.length()) {
            QString cmd = m_commands[m_commandIndex];
            emit command(cmd);
            // qDebug() << "CMD" << cmd;

            for (int i = 0; i<m_commands.length(); i++) {
                auto btn = m_commandsWidgets[i];
                QString color = "";

                if (i == m_commandIndex) {
                    color = "#159";
                } else if (i < m_commandIndex) {
                    color = "#0CF";

                }

                btn->setStyleSheet(color.isEmpty() ? "" : "QPushButton { background: " + color + " }");
            }
        } else {
            for (int i = 0; i<m_commands.length(); i++) {
                auto btn = m_commandsWidgets[i];
                btn->setStyleSheet("QPushButton { background: #0CF }");
            }
            if (m_commandIndex > m_commands.length()) {
                stop();
            }
        }
        m_commandIndex ++;
    }
}
