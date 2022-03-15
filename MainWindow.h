#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ev3.h"
#include "ev3motor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    Ev3 *m_ev3 = nullptr;
    QVector<ev3motor*> m_motors;

};
#endif // MAINWINDOW_H
