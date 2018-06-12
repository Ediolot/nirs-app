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
    uint32_t width;  // Full frame width
    uint32_t height; // Full frame height (2x subframes)
    uint32_t bpp;    // Bits per pixel
    Frame<double> dark;
    Frame<double> gain;
    Frame<double> basal;
    QVector<Frame<double>> frames;
    QString path;

public:
    explicit Experiment(const QString &path);
    ~Experiment();

    void load(const QString &path);
    bool calculateBasal(uint32_t msStart, uint32_t msEnd);
    const Frame<double>& getBasal() const;

signals:
    void loadPercent(float);
    void loadStarted();
    void loadReady();
    void fileError(QString);

private:
    int getStandardBpp(int bpp);
    void run() override;
    int getFrameAt(uint32_t ms) const;
    Frame<double> hSaturation(Frame<double> img1, Frame<double> img2) const;
 };

#endif // EXPERIMENT_H
