#include "mainwindow.h"
#include "mainview.h"

#include <QApplication>
MainView *view;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTime now = QTime::currentTime();
    qsrand(now.msec());

    MainWindow w;
    w.show();

    return a.exec();
}
