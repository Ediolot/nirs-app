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
    enum Params {
        TYPE = 0, AUTOFS = 1, ORDER = 2, FC = 3, FS = 4, RIPPLE = 5, WILDCARD = 6, BANDWIDTH = 7
    };

    explicit Signal(const QString& name = "");
    explicit Signal(const Signal& other);
    explicit Signal(const QVector<double>& other);
    Signal(Signal&& other);
    Signal(QVector<double>&& other);

    QVariantList toQVariantList() const;
    Signal applyFilter(const QString& name, const QVariantList& params) const;

    QString getName() const;
    void setName(const QString& name);

    static void generateCSV(const QString& filepath, const QVector<const Signal*>& args, char separator = ',');

    Signal& operator=(Signal&& other);
    Signal& operator=(const Signal& other);

private:

    Dsp::Params setupParams(const QString& name, const QString& type, const QVariantList& params) const;
    Dsp::Filter* resolveFilter(const QString& name, const QString& type, int order) const;

    template<int T>
    Dsp::Filter* resolveFilterOrder(const QString& name, const QString& type) const {
            if (name == "hp" && type == "BUTTER")       return new Dsp::FilterDesign<Dsp::Butterworth::Design::HighPass<T>, 1>();
       else if (name == "lp" && type == "BUTTER")       return new Dsp::FilterDesign<Dsp::Butterworth::Design::LowPass<T> , 1>();
       else if (name == "bp" && type == "BUTTER")       return new Dsp::FilterDesign<Dsp::Butterworth::Design::BandPass<T>, 1>();
       else if (name == "bs" && type == "BUTTER")       return new Dsp::FilterDesign<Dsp::Butterworth::Design::BandStop<T>, 1>();
       else if (name == "hp" && type == "CHEBYSHEV_I")  return new Dsp::FilterDesign<Dsp::ChebyshevI::Design::HighPass<T>, 1>();
       else if (name == "lp" && type == "CHEBYSHEV_I")  return new Dsp::FilterDesign<Dsp::ChebyshevI::Design::LowPass<T> , 1>();
       else if (name == "bp" && type == "CHEBYSHEV_I")  return new Dsp::FilterDesign<Dsp::ChebyshevI::Design::BandPass<T>, 1>();
       else if (name == "bs" && type == "CHEBYSHEV_I")  return new Dsp::FilterDesign<Dsp::ChebyshevI::Design::BandStop<T>, 1>();
       else if (name == "hp" && type == "CHEBYSHEV_II") return new Dsp::FilterDesign<Dsp::ChebyshevII::Design::HighPass<T>, 1>();
       else if (name == "lp" && type == "CHEBYSHEV_II") return new Dsp::FilterDesign<Dsp::ChebyshevII::Design::LowPass<T> , 1>();
       else if (name == "bp" && type == "CHEBYSHEV_II") return new Dsp::FilterDesign<Dsp::ChebyshevII::Design::BandPass<T>, 1>();
       else if (name == "bs" && type == "CHEBYSHEV_II") return new Dsp::FilterDesign<Dsp::ChebyshevII::Design::BandStop<T>, 1>();
       else if (name == "hp" && type == "LEGENDRE")     return new Dsp::FilterDesign<Dsp::Legendre::Design::HighPass<T>, 1>();
       else if (name == "lp" && type == "LEGENDRE")     return new Dsp::FilterDesign<Dsp::Legendre::Design::LowPass<T> , 1>();
       else if (name == "bp" && type == "LEGENDRE")     return new Dsp::FilterDesign<Dsp::Legendre::Design::BandPass<T>, 1>();
       else if (name == "bs" && type == "LEGENDRE")     return new Dsp::FilterDesign<Dsp::Legendre::Design::BandStop<T>, 1>();
       else if (name == "hp" && type == "BESSEL")       return new Dsp::FilterDesign<Dsp::Bessel::Design::HighPass<T>, 1>();
       else if (name == "lp" && type == "BESSEL")       return new Dsp::FilterDesign<Dsp::Bessel::Design::LowPass<T> , 1>();
       else if (name == "bp" && type == "BESSEL")       return new Dsp::FilterDesign<Dsp::Bessel::Design::BandPass<T>, 1>();
       else if (name == "bs" && type == "BESSEL")       return new Dsp::FilterDesign<Dsp::Bessel::Design::BandStop<T>, 1>();
       else if (name == "hp" && type == "ELLIPTIC")     return new Dsp::FilterDesign<Dsp::Elliptic::Design::HighPass<T>, 1>();
       else if (name == "lp" && type == "ELLIPTIC")     return new Dsp::FilterDesign<Dsp::Elliptic::Design::LowPass<T> , 1>();
       else if (name == "bp" && type == "ELLIPTIC")     return new Dsp::FilterDesign<Dsp::Elliptic::Design::BandPass<T>, 1>();
       else if (name == "bs" && type == "ELLIPTIC")     return new Dsp::FilterDesign<Dsp::Elliptic::Design::BandStop<T>, 1>();
       else if (name == "hp" && type == "RBJ_BIQUAD")   return new Dsp::FilterDesign<Dsp::RBJ::Design::HighPass,  1>;
       else if (name == "lp" && type == "RBJ_BIQUAD")   return new Dsp::FilterDesign<Dsp::RBJ::Design::LowPass ,  1>;
       else if (name == "bp" && type == "RBJ_BIQUAD")   return new Dsp::FilterDesign<Dsp::RBJ::Design::BandPass1, 1>;
       else if (name == "bs" && type == "RBJ_BIQUAD")   return new Dsp::FilterDesign<Dsp::RBJ::Design::BandStop,  1>;
       else return nullptr;
    }

};


#endif // SIGNAL_H
