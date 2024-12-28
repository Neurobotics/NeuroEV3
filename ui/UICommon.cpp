#include "UICommon.h"
#include <QDesktopServices>

UICommon::UICommon(QObject *parent) : QObject(parent)
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

void UICommon::ChangeBackground(QWidget *w, QColor color)
{
    QPalette pal(w->palette());
    pal.setColor(QPalette::Window, color);
    w->setAutoFillBackground(true);
    w->setPalette(pal);
}

QPushButton *UICommon::iconedButton(QString iconPath, QSize size, QSize iconSize)
{
    auto btn = new QPushButton();
    btn->setIcon(QIcon(iconPath));
    btn->setIconSize(iconSize);
    btn->setFixedSize(size);
    btn->setContentsMargins(0,0,0,0);
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
}

QPushButton *UICommon::flatButton(QString text, QIcon icon, QString tooltip, QUrl url)
{
    auto btn = new QPushButton(icon, text);
    btn->setFlat(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setToolTip(tooltip);
    if (!url.isEmpty()) {
        connect(btn, &QPushButton::clicked, [=]() {
            QDesktopServices::openUrl(url);
        });
    }
    return btn;
}
