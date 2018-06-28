#include "signal.h"

Signal::Signal(const QString &name)
    : QVector<double>()
    , name(name)
{}

Signal::Signal(const Signal &other)
    : QVector<double>(other)
    , name(other.name)
{}

Signal::Signal(const QVector<double> &other)
    : QVector<double>(other)
    , name("")
{}

Signal::Signal(Signal &&other)
    : QVector<double>(std::move(other))
    , name(std::move(other.name)) // TODO Check
{}

Signal::Signal(QVector<double> &&other)
    : QVector<double>(std::move(other))
    , name("")
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

    Dsp::SimpleFilter <Dsp::ChebyshevI::LowPass<ORDER>, 1> filter;
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

QString Signal::getName() const
{
    return name;
}

void Signal::setName(const QString &name)
{
    this->name = name;
}

void Signal::generateCSV(const QString &filepath, const QVector<const Signal*> &args, char separator)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        throw CantWriteFileException(filepath);
    }

    QTextStream outStream(&file);
    bool anySignalActive = true;
    int i = 0;

    outStream << "frame";
    for (int j = 0; j < args.size(); ++j) {
        outStream << separator << args[i]->getName();
    }
    outStream << "\n";

    while (anySignalActive) {
        outStream << QString::number(i + 1);
        anySignalActive = false;
        for (int j = 0; j < args.size(); ++j) {
            outStream << separator;
            if (args[j]->size() > i) {
                outStream << QString::number(args[j]->at(i));
                anySignalActive = true;
            }
        }
        outStream << "\n";
        i++;
    }
    file.close();
}
