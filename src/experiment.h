#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QDebug>
#include <QString>
#include <QVector>
#include <cassert>
#include <fstream>
#include <QFile>
#include "frame.h"
#include "exceptions/filenotfoundexception.h"

// TODO usar QFile
// TODO usar QString en las excepciones
// TODO file.read devuelve un código de error que hay que leer

class Experiment : public QObject
{
    Q_OBJECT // For QDebug
private:
    uint32_t width;  // Full frame width
    uint32_t height; // Full frame height (2x subframes)
    uint32_t bpp;    // Bits per pixel
    Frame<double> dark;
    Frame<double> gain;
    QVector<Frame<double>> frames;

public:
    explicit Experiment(const QString &path);
    void load(const QString &path);
 };

#endif // EXPERIMENT_H
