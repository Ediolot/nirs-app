#include "mainwindow.h"
#include <QApplication>
#include <iostream>

const int EIGEN_THREADS = 1;

int main(int argc, char *argv[])
{
    Eigen::initParallel();
    Eigen::setNbThreads(EIGEN_THREADS);
    qDebug() << "Eigen, using" << Eigen::nbThreads() << "threads";
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
