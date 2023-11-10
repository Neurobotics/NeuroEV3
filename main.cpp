#include "ui/MainWindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile fileStyle(":/resources/style.css");
    if (fileStyle.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(fileStyle.readAll());
    }

    MainWindow w;
    w.show();
    return a.exec();
}
