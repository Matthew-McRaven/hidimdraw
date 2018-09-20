#include "mainwindow.h"
#include <QApplication>
#include <ndimscene.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    ndimscene* s = new ndimscene(4);
    w.setCentralWidget(s);
    w.show();

    return a.exec();
}
