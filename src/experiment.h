#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QDebug>
#include <QString>
#include <QVector>
#include <cassert>
#include <fstream>
#include <cmath>
#include <QFile>
#include <QThread>
#include <QVariantList>
#include "frame.h"
#include "exceptions/filenotfoundexception.h"
#include "exceptions/framebpptoobig.h"

// TODO usar QFile
// TODO usar QString en las excepciones
// TODO file.read devuelve un código de error que hay que leer

class Experiment : private QThread, public QObject
{
    Q_OBJECT // For QDebug
private:
    const QString TAG_LOAD = "LOAD";

    uint32_t width;  // Full frame width
    uint32_t height; // Full frame height (2x subframes)
    uint32_t bpp;    // Bits per pixel
    Frame<double> dark;
    Frame<double> gain;
    Frame<double> basal;
    QString path;
    int task;
    int msStart;

    // TODO algo mejor ?
    // QVector<Frame<int8_t>>  frames8;
    QVector<Frame<int16_t>> frames;
    // QVector<Frame<int32_t>> frames32;
    // QVector<Frame<int64_t>> frames64;

public:
    explicit Experiment(const QString &path);
    ~Experiment();

    void load(const QString &path);
    void calculateBasal(uint32_t msStart, uint32_t msEnd);
    void maskOperation(Frame<double>& img1, Frame<double>& img2) const;
    void generateSatFrame(int index, uint32_t msStart = 0);
    void calculateAllSatValues(uint32_t msStart = 0);
    const Frame<double>& getBasal() const;

signals:
    void taskUpdate(QString, double);
    void taskStart(QString);
    void taskComplete(QString);

    void fileError(QString);
    void satFrame(QByteArray, int, int, int, double, double); // frame, width, height, index, meanTop, meanBot
    void satValues(QVariantList, QVariantList);

private:
    int getStandardBpp(int bpp);
    void run() override;
    int getFrameAt(uint32_t ms) const;
    Frame<double> hSaturation(Frame<double> img1, Frame<double> img2) const;
 };

#endif // EXPERIMENT_H
