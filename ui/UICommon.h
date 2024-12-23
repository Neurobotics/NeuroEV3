#ifndef UICOMMON_H
#define UICOMMON_H

#include <QObject>
#include <QProgressBar>

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
};

#endif // UICOMMON_H
