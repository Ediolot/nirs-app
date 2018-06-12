#include "experiment.h"

Experiment::Experiment(const QString &path)
    : QThread()
    , path("")
{
    load(path);
}

Experiment::~Experiment()
{
    QThread::requestInterruption();
    QThread::wait();
}

void Experiment::load(const QString &path)
{
    this->path = path;
    QThread::start();
}

void Experiment::calculateBasal(uint32_t msStart, uint32_t msEnd)
{
    if (frames.isEmpty()) {
        basal = Frame<double>();
    }

    int firstIndex = getFrameAt(msStart);
    int lastIndex  = getFrameAt(msEnd);
    Frame<double> top;
    Frame<double> bottom;
    Frame<double> aux = Frame<double>(width, height);

    for (int i = firstIndex; i < lastIndex; ++i) {
        aux += frames[i].cast<double>();
    }
    aux /= (lastIndex - firstIndex);
    aux = (aux - dark) * gain;
    aux.verticalSplit(height / 2, top, bottom);
    basal = hSaturation(top, bottom);
}

Frame<double> Experiment::generateSatFrame(int index, uint32_t msStart)
{
    int indexStart = getFrameAt(msStart);
    Frame<double> aux;
    Frame<double> top;
    Frame<double> bottom;

    aux = frames[indexStart + index].cast<double>();
    aux = (aux - dark) * gain;
    aux.verticalSplit(height / 2, top, bottom);
    aux = hSaturation(top, bottom);
    aux -= basal;
    return std::move(aux);
}

const Frame<double> &Experiment::getBasal() const
{
    return basal;
}

int Experiment::getStandardBpp(int bpp)
{
         if (bpp <=  8) return 8;
    else if (bpp <= 16) return 16;
    else if (bpp <= 32) return 32;
    else if (bpp <= 64) return 64;
    else                return bpp;
}

// Es conveniente que run no haga throw de ninguna excepción
void Experiment::run()
{
    try {
        assert(sizeof(float ) == 4); // 32 bit Float  // TODO que ambos sean por configuración
        assert(sizeof(double) == 8); // 64 bit Double

        QFile file(path);

        if (!file.open(QIODevice::ReadOnly)) {
            throw FileNotFoundException(path);
        }

        // Width, Height, Bpp. If any fails, throw exception
        emit loadStarted();
        if (file.read(reinterpret_cast<char*>(&height), sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
        if (file.read(reinterpret_cast<char*>(&width),  sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
        if (file.read(reinterpret_cast<char*>(&bpp),    sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
        bpp = getStandardBpp(bpp);

        // Frames
        dark = Frame<float>(file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();
        gain = Frame<float>(file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();

        int nframes = (file.size() - file.pos()) / (width * height * bpp / 8);
        for (int i = 0; (i < nframes) && !QThread::isInterruptionRequested(); ++i) {
            frames.push_back(Frame<int16_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //     if (bpp <=  8) frames.push_back(Frame< int8_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 16) frames.push_back(Frame<int16_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 32) frames.push_back(Frame<int32_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 64) frames.push_back(Frame<int64_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else throw FrameBPPTooBig(bpp);

            emit loadPercent((i + 1.0f) / nframes);
        }

        file.close();
        emit loadReady();

    // The error will be controlled by the javascript interface. So we can just emit it
    } catch (std::exception &e) {
        emit fileError(QString(e.what()));
    } catch (...) {
        emit fileError("Fatal error");
    }
}

int Experiment::getFrameAt(uint32_t ms) const
{
    uint64_t ns = uint64_t(ms) * 1000000UL;
    uint64_t firstTimestamp = frames[0].getTimestamp();
    uint64_t currentNs = 0;
    int index = 0;

    while (currentNs < ns) {
        if (++index >= frames.size())
            qDebug() << "Error size"; // TODO exception

        currentNs = frames[index].getTimestamp() - firstTimestamp;
    }
    return index;
}

Frame<double> Experiment::hSaturation(Frame<double> img1, Frame<double> img2) const
{
    if ((img1.getWidth() != img2.getWidth()) || (img1.getHeight() != img2.getHeight()))
        qDebug() << "size mismatch"; // TODO exception

    // LAMBDA1 = 852;
    const double CEM_HBO2_LAMBDA1 = 1062;
    const double CEM_HB_LAMBDA1 = 691.08;
    // LAMBDA2 = 630;
    const double CEM_HBO2_LAMBDA2 = 610;
    const double CEM_HB_LAMBDA2 = 5148.8;

    img1.applyLog();
    img2.applyLog();
    Frame<double> aux1 = img1 / img2;
    Frame<double> aux2 = Frame<double>(aux1);

    aux1 *= CEM_HB_LAMBDA2;
    aux1 -= CEM_HB_LAMBDA1;
    aux2 *= CEM_HB_LAMBDA2 - CEM_HBO2_LAMBDA2;
    aux2 += CEM_HBO2_LAMBDA1 - CEM_HB_LAMBDA1;
    return aux1 / aux2;
}


