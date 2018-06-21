#include "experiment.h"

Experiment::Experiment()
    : QObject()
    , path("")
{}

Experiment::Experiment(const QString &path)
    : QObject()
    , path("")
{
    load(path);
}

void Experiment::load(const QString &path)
{
    assert(sizeof(float ) == 4); // 32 bit Float  // TODO que ambos sean por configuración
    assert(sizeof(double) == 8); // 64 bit Double
    this->path = path;

    emit taskStart(TAG_LOAD);
    QFile* file = new QFile(path);

    TaskLauncher::create([=](){
        if (!file->open(QIODevice::ReadOnly)) {
            throw FileNotFoundException(path);
        }

        // Width, Height, Bpp. If any fails, throw exception
        if (file->read(reinterpret_cast<char*>(&height), sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
        if (file->read(reinterpret_cast<char*>(&width),  sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
        if (file->read(reinterpret_cast<char*>(&bpp),    sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
        bpp = getStandardBpp(bpp);

        // Frames
        dark = Frame<float>(*file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();
        gain = Frame<float>(*file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();

        int nframes = (file->size() - file->pos()) / (width * height * bpp / 8);
        frames.reserve(nframes);
        for (int i = 0; (i < nframes); ++i) {
            frames.push_back(Frame<int16_t>(*file, width, height, FrameConstants::HAS_TIMESTAMP));
            //     if (bpp <=  8) frames.push_back(Frame< int8_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 16) frames.push_back(Frame<int16_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 32) frames.push_back(Frame<int32_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 64) frames.push_back(Frame<int64_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else throw FrameBPPTooBig(bpp);

            if (i % 10 == 0) { // TODO constant (Hay más)
                emit taskUpdate(TAG_LOAD, (i + 1.0f) / nframes);
            }
        }

    }, [=](){
        file->close();
        delete file;
        emit taskComplete(TAG_LOAD);
    }, [=](QString err){
        emit fileError(err);
    });
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

    qDebug() << indexStart + index;
    qDebug() << aux.getData()(0, 0) <<  aux.getData()(0, 1) <<  aux.getData()(0, 2);
    qDebug() << aux.getData()(1, 0);
    qDebug() << aux.getData()(2, 0);

    emit satFrame(aux.toQVariantList(FrameConstants::COLUM_MAJOR),
                  aux.getWidth(),
                  aux.getHeight(),
                  index,
                  topMean,
                  bottomMean);
}

void Experiment::calculateAllSatValues(uint32_t msStart)
{
    static int THREADS = 2;
    int first = getFrameAt(msStart);
    int length = frames.size() - first;
    QAtomicInt *elementsDone = new QAtomicInt(0);
    QVector<std::function<void (void)>> tasks;
    QVector<QVariantList>* meanListsA = new QVector<QVariantList>();
    QVector<QVariantList>* meanListsB = new QVector<QVariantList>();

    emit taskStart(TAG_PROCESS);

    for (int th = 0; th < THREADS; ++th) {
        meanListsA->push_back(QVariantList());
        meanListsB->push_back(QVariantList());
        meanListsA->back().reserve(length / THREADS);
        meanListsB->back().reserve(length / THREADS);

        tasks.push_back([=](){
            Frame<double> aux;
            Frame<double> top;
            Frame<double> bottom;
            int start = first + length * th / THREADS;
            int end   = first + length * (th + 1) / THREADS;

            for (int i = start; i < end; ++i) {
                aux = frames[i].cast<double>();
                aux = (aux - dark) * gain;
                aux.verticalSplit(height / 2, top, bottom);
                maskOperation(top, bottom);
                (*meanListsA)[th].append(top.getData().mean());
                (*meanListsB)[th].append(bottom.getData().mean());
                if (i % 10 == 0) {
                    (*elementsDone) += 10;
                    emit taskUpdate(TAG_PROCESS, *elementsDone / double(length));
                }
            }
        });
    }

    TaskLauncher::afterAll(tasks, [=](){
        QVariantList meanListA;
        QVariantList meanListB;
        meanListA.reserve(length);
        meanListB.reserve(length);
        for (QVariantList& list : *meanListsA) {
            meanListA.append(list);
        }
        for (QVariantList& list : *meanListsB) {
            meanListB.append(list);
        }
        emit taskComplete(TAG_PROCESS);
        emit satValues(meanListA, meanListB);
        delete elementsDone;
        delete meanListsA;
        delete meanListsB;
    });
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


