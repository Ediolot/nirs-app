#include "mainwindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    Eigen::initParallel();
    Eigen::setNbThreads(1);
    qDebug() << "EIGEN THREADS" << Eigen::nbThreads();
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // TODO ?
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); // TODO ?
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
