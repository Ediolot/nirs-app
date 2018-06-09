#include "mainwindow.h"
#include <QApplication>
#include <iostream>

#include <QtWebChannel>
//#include <QWebSocketServer>

#include "frame.h"
#include "experiment.h"

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //Experiment expe("experiment_2.exp");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
