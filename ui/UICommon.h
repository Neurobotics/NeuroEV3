#ifndef UICOMMON_H
#define UICOMMON_H

#include <QUrl>
#include <QIcon>
#include <QObject>
#include <QPushButton>
#include <QProgressBar>
#include "ui/IconLabel.h"

static QColor state1 = QColor(0xCC3300);
static QColor state2 = QColor(0x33CC00);
static QColor state3 = QColor(0x2266AA);
static QColor state4 = QColor(0xCCCC00);
static QColor state5 = QColor(0xFF00FF);
static QColor state6 = QColor(0x00CCFF);
static QColor state7 = QColor(0xFF8800);
static QColor state8 = QColor(0xFF6688);
static QColor stateGray = QColor(0xCCCCCC);

class UICommon : public QObject
{
    Q_OBJECT
public:
    static UICommon* Instance();

    static QString motorString(int motorIndex);

    QIcon iconSocket;
    QIcon iconSocketActive;
    QIcon iconConnected;
    QIcon iconDisconnected;

    IconLabel *motorSocket(int motorIndex);
    IconLabel *connectStatusWidget();

    static QProgressBar *progressBar(QString color);

    static QProgressBar *meditationBar();
    static QProgressBar *concentrationBar();

    inline static QString ColorMeditation = "#7C2";
    inline static QString ColorConcentration = "#C30";

    static void ChangeBackground(QWidget *w, QColor color);

    static QPushButton *iconedButton(QString iconPath, QSize size = QSize(42, 42), QSize iconSize = QSize(32, 32));
    static QPushButton *flatButton(QString text, QIcon icon = QIcon(), QString tooltip = "", QUrl url = QUrl());

    QColor biosignalStateColor(int state);

protected:
    explicit UICommon(QObject *parent = nullptr);
    static UICommon* _instance;
};

#endif // UICOMMON_H
