#include <QApplication>
#include "gui.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("BootUSB");
    w.resize(700,300);
    w.show();
    return a.exec();
}