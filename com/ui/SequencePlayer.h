#ifndef SEQUENCEPLAYER_H
#define SEQUENCEPLAYER_H

#include <QWidget>
#include "ui/nscrollviewer.h"
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QTimer>

class SequencePlayer : public QWidget
{
    Q_OBJECT
public:
    explicit SequencePlayer(QWidget *parent = nullptr);

    void addCommand(QString cmd, QString view);

    void play(bool on = true);
    void playFrom(int index = 0, bool on = true);
    void stop();

    void enableSequence(bool on);
    bool sequenceEnabled();

    void setInterval(int ms);

    void clearCommands();


signals:
    void command(QString cmd);
    void intervalChanged(int ms);

protected:
    QList<QString> m_commands;
    QList<QWidget*> m_commandsWidgets;
    NScrollViewer *m_commandsScroll = nullptr;

    QDoubleSpinBox *m_spinTimeStep = nullptr;

    QWidget *m_sequenceWidget = nullptr;

    QPushButton *m_btnCheck = nullptr;
    QPushButton *m_btnPlay = nullptr;
    QPushButton *m_btnStop = nullptr;

    QTimer *m_timer = nullptr;

    int m_intervalMs = 1000;

    bool m_isPlaying = false;
    int m_commandIndex = 0;

    QPushButton *squareButton(QIcon icon, QString text = "", QString tooltip = "");

    QString m_btnStyle = "QPushButton { font-size: 20px; padding: 0; border: 1px solid #999; border-radius: 4px";

    void nextCommand();
};

#endif // SEQUENCEPLAYER_H
