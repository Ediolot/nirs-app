#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QDebug>
#include <QElapsedTimer>
#include <QString>
#include <QVector>
#include <cassert>
#include <fstream>
#include <cmath>
#include <QFile>
#include <QThread>
#include <QVariantList>
#include "frame.h"
#include "task.h"
#include "exceptions/filenotfoundexception.h"
#include "exceptions/framebpptoobig.h"
#include "exceptions/frametoobigexception.h"
#include "exceptions/badindexforframeexception.h"
#include "exceptions/sizeframemissmatch.h"
#include "tasklauncher.h"
#include "signal.h"
#include <chrono>
#include <QAtomicInt>
#include <algorithm>

class Experiment : public QObject
{
    Q_OBJECT // For QDebug
private:
    const QString TAG_LOAD = "LOAD";
    const QString TAG_PROCESS = "PROCESS";
    const QString TAG_BASALGEN = "BASAL";
    const uint32_t MAX_DIMENSION = 5000;
    const int UPDATE_INTERVAL = 10;
    const double MASK_VALUE = 0.9;

    uint32_t width;  // Full frame width
    uint32_t height; // Full frame height (2x subframes)
    uint32_t bpp;    // Bits per pixel
    Frame<double> dark;
    Frame<double> gain;
    Frame<double> basal;
    QString path;
    int task;
    int msStart;

    QVector<Frame<double>> frames;

    Signal A;
    Signal B;
    double sampleFreq;

public:
    Experiment();
    explicit Experiment(const QString &path);

    void load(const QString &path);
    void generateBasalFrame(uint32_t msStart, uint32_t msEnd);
    void maskOperation(Frame<double>& img1, Frame<double>& img2) const;
    void generateSatFrame(int index, uint32_t msStart = 0);
    void calculateAllSatValues(int roiX0, int roiY0, int roiX1, int roiY1, uint32_t msStart = 0);
    void exportSatValuesToCSV(const QString& path, char separator);
    const Frame<double>& getBasal() const;

signals:
    void taskUpdate(QString, double);
    void taskStart(QString);
    void taskComplete(QString);

    void fileError(QString);
    void satFrame(QVariantList, int, int, int, int, double, double); // frame, width, height, index, max, meanTop, meanBot
    void basalFrame(QVariantList, int, int); // frame, width, height
    void satValues(QVariantList, QVariantList);

private:
    int getStandardBpp(int bpp);
    int getFrameAt(uint32_t ms) const;
    Frame<double> hSaturation(Frame<double> img1, Frame<double> img2) const;
 };

#endif // EXPERIMENT_H
