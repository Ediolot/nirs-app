#include "mainwindow.h"
#include <QApplication>
#include <iostream>
//#include <Eigen/Core>
//#include <QDebug>
//#include <chrono>
//#include <thread>
//#include <vector>
//#include <iostream>
//#include "frame.h"

//using namespace std;

int main(int argc, char *argv[])
{
    Eigen::initParallel();
    Eigen::setNbThreads(1);
    qDebug() << "THREADS" << Eigen::nbThreads();

//    int n = 10;
//    int sz = 2000;
//    int bothTotalTime = 0;
//    int t1TotalTime = 0;
//    int t2TotalTime = 0;
//    int tests = 50;
//    int t1SzStart = 0;
//    int t2SzStart = sz / 2;
//    int t1SzEnd   = sz / 2;
//    int t2SzEnd   = sz;

//    // GENERATE DATA
//    QVector<double> data;
//    for (int i = 0; i < n*n; ++i) {
//        data.push_back(i);
//    }

//    Frame<double> dark(data, n, n);
//    Frame<double> gain(data, n, n);
//    vector<Frame<double>> v;
//    for (int i = 0; i < sz; ++i) {
//        v.push_back(Frame<double>(data, n, n));
//    }

//    // RUN IN TWO THREADS
//    for (int i = 0; i < tests; ++i) {
//        chrono::steady_clock::time_point t1End;
//        chrono::steady_clock::time_point t2End;
//        chrono::steady_clock::time_point bothEnd;
//        chrono::steady_clock::time_point start = chrono::steady_clock::now();

//        std::thread t1([=, &v, &t1End](){
//            for (int i = t1SzStart; i < t1SzEnd; ++i) {
//                v[i] = (v[i] - dark) * gain;
//            }
//            t1End = chrono::steady_clock::now();
//        });

//        std::thread t2([=, &v, &t2End](){
//            for (int i = t2SzStart; i < t2SzEnd; ++i) {
//                v[i] = (v[i] - dark) * gain;
//            }
//            t2End = chrono::steady_clock::now();
//        });

//        t1.join();
//        t2.join();
//        bothEnd = chrono::steady_clock::now();
//        bothTotalTime += chrono::duration_cast<chrono::microseconds>(bothEnd - start).count();
//        t1TotalTime   += chrono::duration_cast<chrono::microseconds>(t1End - start).count();
//        t2TotalTime   += chrono::duration_cast<chrono::microseconds>(t2End - start).count();
//        std::cout << i << std::endl;
//    }

//    double bothAvg = bothTotalTime / double(tests);
//    double t1Avg = t1TotalTime / double(tests);
//    double t2Avg = t2TotalTime / double(tests);
//    double sumTime = t1Avg + t2Avg;
//    std::cout << "Avg total us: " << bothAvg << std::endl;
//    std::cout << "Sum time us:  " << sumTime << std::endl;
//    std::cout << "Time saved:   " << int((sumTime - bothAvg) * 100 / sumTime) << "%" << std::endl;
//    std::cout << "Avg t1 us:    " << t1Avg << std::endl;
//    std::cout << "Avg t2 us:    " << t2Avg << std::endl;
//    return 0;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // TODO ?
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); // TODO ?
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
