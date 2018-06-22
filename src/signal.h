#ifndef SIGNAL_H
#define SIGNAL_H

#include <QDebug>
#include <QObject>
#include <QVector>
#include <QVariantList>
#include <DspFilters/Dsp.h>
#include <utility>

class Signal : public QVector<double>
{
public:
    static const int ORDER = 3;

    Signal();
    Signal(const Signal& other);
    Signal(const QVector<double>& other);
    Signal(Signal&& other);
    Signal(QVector<double>&& other);

    QVariantList toQVariantList() const;
    Signal runLowPassFilter(int sampleRate, int cutFreq, int rippleDB) const;
    Signal runHighPassFilter(int sampleRate, int cutFreq, int rippleDB) const;
    Signal runBandPassFilter(int sampleRate, int centerFreq, int bandWidth, int rippleDB) const;
    Signal runBandStopFilter(int sampleRate, int centerFreq, int bandWidth, int rippleDB) const;
};

#endif // SIGNAL_H
