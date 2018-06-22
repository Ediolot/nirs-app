#include "signal.h"

Signal::Signal()
    : QVector<double>()
{}

Signal::Signal(const Signal &other)
    : QVector<double>(other)
{}

Signal::Signal(const QVector<double> &other)
    : QVector<double>(other)
{}

Signal::Signal(Signal &&other)
    : QVector<double>(std::move(other))
{}

Signal::Signal(QVector<double> &&other)
    : QVector<double>(std::move(other))
{}

QVariantList Signal::toQVariantList() const
{
    int sz = QVector::size();
    QVariantList copy;

    copy.reserve(sz);
    for (int i = 0; i < sz; ++i) {
        copy.append(QVariant(QVector::at(i)));
    }
    return copy;
}

Signal Signal::runLowPassFilter(int sampleRate, int cutFreq, int rippleDB) const
{
    Signal copy(*this);
    double* channels[1];
    channels[0] = copy.data();

    Dsp::SimpleFilter <Dsp::ChebyshevI::HighPass<ORDER>, 1> filter;
    filter.setup(ORDER, sampleRate, cutFreq, rippleDB);
    filter.process(copy.size(), channels);

    return copy;
}

Signal Signal::runHighPassFilter(int sampleRate, int cutFreq, int rippleDB) const
{
    Signal copy(*this);
    double* channels[1];
    channels[0] = copy.data();

    Dsp::SimpleFilter <Dsp::ChebyshevI::HighPass<ORDER>, 1> filter;
    filter.setup(ORDER, sampleRate, cutFreq, rippleDB);
    filter.process(copy.size(), channels);

    return copy;
}

Signal Signal::runBandPassFilter(int sampleRate, int centerFreq, int bandWidth, int rippleDB) const
{
    Signal copy(*this);
    double* channels[1];
    channels[0] = copy.data();

    Dsp::SimpleFilter <Dsp::ChebyshevI::BandPass<ORDER>, 1> filter;
    filter.setup(ORDER, sampleRate, centerFreq, bandWidth, rippleDB);
    filter.process(copy.size(), channels);

    return copy;
}

Signal Signal::runBandStopFilter(int sampleRate, int centerFreq, int bandWidth, int rippleDB) const
{
    Signal copy(*this);
    double* channels[1];
    channels[0] = copy.data();

    Dsp::SimpleFilter <Dsp::ChebyshevI::BandStop<ORDER>, 1> filter;
    filter.setup(ORDER, sampleRate, centerFreq, bandWidth, rippleDB);
    filter.process(copy.size(), channels);

    return copy;
}
