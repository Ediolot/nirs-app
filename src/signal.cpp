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
    , name(std::move(other.name))
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

Signal Signal::applyFilter(const QString &name, const QVariantList &params) const
{
    Signal copy(*this);
    double* channels[1];
    channels[0] = copy.data();

    Dsp::Filter* filter = nullptr;
    QString type     = params[Params::TYPE].toString();
    int order        = params[Params::ORDER].toInt();

    try {
        filter = resolveFilter(name, type, order);
        filter->setParams(setupParams(name, type, params));
        filter->process(copy.size(), channels);
    } catch (std::exception e) {
        // TODO this is temporal. An emergent window is required
        qDebug() << QString::fromUtf8(e.what());
    }

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

Signal &Signal::operator=(Signal &&other)
{
    QVector::operator =(std::move(other));
    name = std::move(other.name);
    return *this;
}

Signal &Signal::operator=(const Signal &other)
{
    QVector::operator =(other);
    name = other.name;
    return *this;
}

Dsp::Params Signal::setupParams(const QString &name, const QString &type, const QVariantList &params) const
{
   Dsp::Params p;
   int order     = params[Params::ORDER].toInt();
   int fc        = params[Params::FC].toDouble();
   int fs        = params[Params::FS].toDouble();
   int ripple    = params[Params::RIPPLE].toDouble();
   int wildcard  = params[Params::WILDCARD].toDouble();
   int bandwidth = params[Params::BANDWIDTH].toDouble();

       if (name == "hp" && type == "BUTTER")       { p[0] = fs; p[1] = order; p[2] = fc; }
  else if (name == "lp" && type == "BUTTER")       { p[0] = fs; p[1] = order; p[2] = fc; }
  else if (name == "bp" && type == "BUTTER")       { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; }
  else if (name == "bs" && type == "BUTTER")       { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; }
  else if (name == "hp" && type == "CHEBYSHEV_I")  { p[0] = fs; p[1] = order; p[2] = fc; p[3] = ripple; }
  else if (name == "lp" && type == "CHEBYSHEV_I")  { p[0] = fs; p[1] = order; p[2] = fc; p[3] = ripple; }
  else if (name == "bp" && type == "CHEBYSHEV_I")  { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; p[4] = ripple; }
  else if (name == "bs" && type == "CHEBYSHEV_I")  { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; p[4] = ripple; }
  else if (name == "hp" && type == "CHEBYSHEV_II") { p[0] = fs; p[1] = order; p[2] = fc; p[3] = wildcard; }
  else if (name == "lp" && type == "CHEBYSHEV_II") { p[0] = fs; p[1] = order; p[2] = fc; p[3] = wildcard; }
  else if (name == "bp" && type == "CHEBYSHEV_II") { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; p[3] = wildcard; }
  else if (name == "bs" && type == "CHEBYSHEV_II") { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; p[3] = wildcard; }
  else if (name == "hp" && type == "LEGENDRE")     { p[0] = fs; p[1] = order; p[2] = fc; }
  else if (name == "lp" && type == "LEGENDRE")     { p[0] = fs; p[1] = order; p[2] = fc; }
  else if (name == "bp" && type == "LEGENDRE")     { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; }
  else if (name == "bs" && type == "LEGENDRE")     { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; }
  else if (name == "hp" && type == "BESSEL")       { p[0] = fs; p[1] = order; p[2] = fc; }
  else if (name == "lp" && type == "BESSEL")       { p[0] = fs; p[1] = order; p[2] = fc; }
  else if (name == "bp" && type == "BESSEL")       { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; }
  else if (name == "bs" && type == "BESSEL")       { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; }
  else if (name == "hp" && type == "ELLIPTIC")     { p[0] = fs; p[1] = order; p[2] = fc; p[3] = ripple; p[4] = wildcard; }
  else if (name == "lp" && type == "ELLIPTIC")     { p[0] = fs; p[1] = order; p[2] = fc; p[3] = ripple; p[4] = wildcard; }
  else if (name == "bp" && type == "ELLIPTIC")     { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; p[4] = ripple; p[5] = wildcard; }
  else if (name == "bs" && type == "ELLIPTIC")     { p[0] = fs; p[1] = order; p[2] = fc; p[3] = bandwidth; p[4] = ripple; p[5] = wildcard; }
  else if (name == "hp" && type == "RBJ_BIQUAD")   { p[0] = fs; p[1] = fc; p[2] = wildcard; }
  else if (name == "lp" && type == "RBJ_BIQUAD")   { p[0] = fs; p[1] = fc; p[2] = wildcard; }
  else if (name == "bp" && type == "RBJ_BIQUAD")   { p[0] = fs; p[1] = fc; p[2] = bandwidth; }
  else if (name == "bs" && type == "RBJ_BIQUAD")   { p[0] = fs; p[1] = fc; p[2] = bandwidth; }
       return p;
}

Dsp::Filter *Signal::resolveFilter(const QString &name, const QString &type, int order) const
{
   switch (order) {
   default: return nullptr;
   case  1: return resolveFilterOrder< 1>(name, type);
   case  2: return resolveFilterOrder< 2>(name, type);
   case  3: return resolveFilterOrder< 3>(name, type);
   case  4: return resolveFilterOrder< 4>(name, type);
   case  5: return resolveFilterOrder< 5>(name, type);
   case  6: return resolveFilterOrder< 6>(name, type);
   case  7: return resolveFilterOrder< 7>(name, type);
   case  8: return resolveFilterOrder< 8>(name, type);
   case  9: return resolveFilterOrder< 9>(name, type);
   case 10: return resolveFilterOrder<10>(name, type);
   }
}
