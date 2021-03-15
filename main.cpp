#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("tgtsml"));

    try {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
    }
    catch (const std::bad_alloc &) {
           return EXIT_FAILURE;
       }

}
