#include "mainwindow.h"
#include <QApplication>

using namespace oscilloscope;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
