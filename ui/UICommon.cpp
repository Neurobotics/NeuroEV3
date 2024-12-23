#include "UICommon.h"

UICommon::UICommon(QObject *parent)
    : QObject{parent}
{}

QProgressBar *UICommon::progressBar(QString color)
{
    auto progress = new QProgressBar();
    progress->setRange(0, 100);
    progress->setStyleSheet("QProgressBar { text-align: center; border: 2px solid grey; border-radius: 5px; background-color: #FFF; } "
                            "QProgressBar::chunk { background-color: " + color + "; padding: 4px; }");
    return progress;
}

QProgressBar *UICommon::meditationBar()
{
    return progressBar(ColorMeditation);
}

QProgressBar *UICommon::concentrationBar()
{
    return progressBar(ColorConcentration);
}
