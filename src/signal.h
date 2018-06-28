#ifndef SIGNAL_H
#define SIGNAL_H

#include <QDebug>
#include <QObject>
#include <QVector>
#include <QVariantList>
#include <DspFilters/Dsp.h>
#include <QFile>
#include <QTextStream>
#include <utility>
#include "exceptions/cantwritefileexception.h"

class Signal : public QVector<double>
{
private:
    QString name;

public:
    static const int ORDER = 3;

    explicit Signal(const QString& name = "");
    explicit Signal(const Signal& other);
    explicit Signal(const QVector<double>& other);
    Signal(Signal&& other);
    Signal(QVector<double>&& other);

    QVariantList toQVariantList() const;
    Signal runLowPassFilter(int sampleRate, int cutFreq, int rippleDB) const;
    Signal runHighPassFilter(int sampleRate, int cutFreq, int rippleDB) const;
    Signal runBandPassFilter(int sampleRate, int centerFreq, int bandWidth, int rippleDB) const;
    Signal runBandStopFilter(int sampleRate, int centerFreq, int bandWidth, int rippleDB) const;

    QString getName() const;
    void setName(const QString& name);

    static void generateCSV(const QString& filepath, const QVector<const Signal*>& args, char separator = ',');
};

#endif // SIGNAL_H
