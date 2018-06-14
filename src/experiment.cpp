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

void Experiment::maskOperation(Frame<double> &img1, Frame<double> &img2) const
{
    auto m1 = img1.getData();
    auto m2 = img2.getData();

    for (int row = 0; row < m1.rows(); ++row) {
        for (int col = 0; col < m1.cols(); ++col) {
            if (m1(row, col) > 0.9 || m2(row, col) > 0.9) {// TODO constant
                img1.set(row, col, 0);
                img2.set(row, col, 0);
            }
        }
    }
}

void Experiment::generateSatFrame(int index, uint32_t msStart)
{
    int indexStart = getFrameAt(msStart);
    Frame<double> aux;
    Frame<double> top;
    Frame<double> bottom;
    double topMean;
    double bottomMean;

    aux = frames[indexStart + index].cast<double>();
    aux = (aux - dark) * gain;
    aux.verticalSplit(height / 2, top, bottom);
    maskOperation(top, bottom);
    topMean = top.getData().mean();
    bottomMean = bottom.getData().mean();

    aux = hSaturation(top, bottom);
    aux -= basal;
    aux *= 100;

    emit satFrame(aux.toIndexed8Base64(FrameConstants::COLUM_MAJOR),
                  aux.getWidth(),
                  aux.getHeight(),
                  index,
                  topMean,
                  bottomMean);
}

void Experiment::calculateAllSatValues(uint32_t msStart)
{
    /* 1" 58'
    QVariantList meanA;
    QVariantList meanB;
    for (int i = getFrameAt(msStart); i < frames.size(); ++i) {
        Frame<double> aux;
        Frame<double> top;
        Frame<double> bottom;

        aux = frames[i].cast<double>();
        aux = (aux - dark) * gain;
        aux.verticalSplit(height / 2, top, bottom);
        maskOperation(top, bottom);
        meanA.append(top.getData().mean());
        meanB.append(bottom.getData().mean());
    }
    emit satValues(meanA, meanB);
    */
    // 1" 17'
    QVariantList meanA_1, meanA_2;
    QVariantList meanB_1, meanB_2;
    int start = getFrameAt(msStart);
    int length = frames.size() - start;

    meanA_1.reserve(length);
    meanB_1.reserve(length);
    meanA_2.reserve(length / 2);
    meanB_2.reserve(length / 2);

    QAtomicInt threads = 1;
    QAtomicInt elementsDone = 0;

    emit taskComplete(TAG_PROCESS);
    TaskLauncher([&](){
        for (int i = start; i < (start + length / 2); ++i) {
            Frame<double> aux;
            Frame<double> top;
            Frame<double> bottom;

            aux = frames[i].cast<double>();
            aux = (aux - dark) * gain;
            aux.verticalSplit(height / 2, top, bottom);
            maskOperation(top, bottom);
            meanA_1.append(top.getData().mean());
            meanB_1.append(bottom.getData().mean());
            if (i % 10 == 0) {
                elementsDone++;
                emit taskUpdate(TAG_PROCESS, elementsDone / frames.size());
            }
        }
    });
    for (int i = (start + length / 2); i < frames.size(); ++i) {
        Frame<double> aux;
        Frame<double> top;
        Frame<double> bottom;

        aux = frames[i].cast<double>();
        aux = (aux - dark) * gain;
        aux.verticalSplit(height / 2, top, bottom);
        maskOperation(top, bottom);
        meanA_2.append(top.getData().mean());
        meanB_2.append(bottom.getData().mean());
        if (i % 10 == 0) {
            elementsDone++;
            emit taskUpdate(TAG_PROCESS, elementsDone / frames.size());
        }
    }
    while (threads > 0) {
        QThread::msleep(10);
    }

    emit taskComplete(TAG_PROCESS);
    meanA_1.append(meanA_2);
    meanB_1.append(meanB_2);
    emit satValues(meanA_1, meanB_1);
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
    emit taskStart(TAG_LOAD);
    try {
        assert(sizeof(float ) == 4); // 32 bit Float  // TODO que ambos sean por configuración
        assert(sizeof(double) == 8); // 64 bit Double

        QFile file(path);

        if (!file.open(QIODevice::ReadOnly)) {
            throw FileNotFoundException(path);
        }

        // Width, Height, Bpp. If any fails, throw exception
        if (file.read(reinterpret_cast<char*>(&height), sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
        if (file.read(reinterpret_cast<char*>(&width),  sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
        if (file.read(reinterpret_cast<char*>(&bpp),    sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
        bpp = getStandardBpp(bpp);

        // Frames
        dark = Frame<float>(file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();
        gain = Frame<float>(file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();

        int nframes = (file.size() - file.pos()) / (width * height * bpp / 8);
        nframes = 600;
        for (int i = 0; (i < nframes) && !QThread::isInterruptionRequested(); ++i) {
            frames.push_back(Frame<int16_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //     if (bpp <=  8) frames.push_back(Frame< int8_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 16) frames.push_back(Frame<int16_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 32) frames.push_back(Frame<int32_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 64) frames.push_back(Frame<int64_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else throw FrameBPPTooBig(bpp);

            emit taskUpdate(TAG_LOAD, (i + 1.0f) / nframes);
        }

        file.close();

    // The error will be controlled by the javascript interface. So we can just emit it
    } catch (std::exception &e) {
        emit fileError(QString(e.what()));
    } catch (...) {
        emit fileError("Fatal error");
    }
    emit taskComplete(TAG_LOAD);
}

int Experiment::getFrameAt(uint32_t ms) const
{
    if (ms == 0) {
        return 0;
    }

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


