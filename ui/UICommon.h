#ifndef UICOMMON_H
#define UICOMMON_H

#include <QObject>
#include <QProgressBar>
#include <QPushButton>
#include <QUrl>

class UICommon : public QObject
{
    Q_OBJECT
public:
    explicit UICommon(QObject *parent = nullptr);

    static QProgressBar *progressBar(QString color);

    static QProgressBar *meditationBar();
    static QProgressBar *concentrationBar();

    inline static QString ColorMeditation = "#7C2";
    inline static QString ColorConcentration = "#C30";

    static void ChangeBackground(QWidget *w, QColor color);

    static QPushButton *iconedButton(QString iconPath, QSize size = QSize(42, 42), QSize iconSize = QSize(32, 32));
    static QPushButton *flatButton(QString text, QIcon icon = QIcon(), QString tooltip = "", QUrl url = QUrl());
};

#endif // UICOMMON_H
