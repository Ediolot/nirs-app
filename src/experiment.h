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
    Q_OBJECT

public:
    enum Type {
        MILLIS = 1, FRAMES = 2
    };

private:
    static const constexpr char*  TAG_LOAD     = "LOAD";
    static const constexpr char*  TAG_PROCESS  = "PROCESS";
    static const constexpr char*  TAG_BASALGEN = "BASAL";
    static const constexpr double MASK_VALUE   = 0.9;
    static const uint32_t MAX_DIMENSION   = 5000;
    static const int      UPDATE_INTERVAL = 10;
    static const int      SAT_THREADS     = 2;
    static const int      MS_TO_NS        = 1e6;

    uint32_t width;  // Full frame width
    uint32_t height; // Full frame height (2x subframes)
    uint32_t bpp;    // Bits per pixel
    Frame<double> dark;
    Frame<double> gain;
    Frame<double> basal;
    QString path;
    int task;

    QVector<Frame<double>> frames;

    Signal A;
    Signal B;
    double sampleFreq;

public:
    Experiment();
    explicit Experiment(const QString &path);

    void load(const QString &path);
    void generateBasalFrame(uint32_t start, uint32_t end, Type type = MILLIS);
    void maskOperation(Frame<double>& img1, Frame<double>& img2) const;
    void generateSatFrame(uint32_t pos, Type type = MILLIS);
    void calculateAllSatValues(int roiX0, int roiY0, int roiX1, int roiY1, uint32_t start = 0, Type type = MILLIS);
    void exportSatValuesToCSV(const QString& path, char separator);
    uint64_t getFrameRelativeTimeMS(int index) const;
    uint64_t getExperimentDurationMS() const;
    const Frame<double>& getBasal() const;
    uint64_t frameToMs(uint64_t frame) const;
    uint64_t msToFrame(uint64_t ms) const;
    qint64 maxFrame() const;
    qint64 maxMs() const;

signals:
    void taskUpdate(QString, double);
    void taskStart(QString);
    void taskComplete(QString);

    void fileError(QString);
    void satFrame(QVariantList, int, int, uint64_t, uint64_t, uint64_t, uint64_t, double, double); // frame, width, height, index, max, meanTop, meanBot
    void basalFrame(QVariantList, int, int); // frame, width, height
    void satValues(QVariantList, QVariantList);

private:
    int getStandardBpp(int bpp);
    uint64_t getFrameAt(uint64_t ms) const;
    Frame<double> hSaturation(Frame<double> img1, Frame<double> img2) const;
 };

#endif // EXPERIMENT_H
