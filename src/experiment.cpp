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
            //else if (bpp <= 16) frames.push_back(Frame<int56_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 32) frames.push_back(Frame<int32_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else if (bpp <= 64) frames.push_back(Frame<int64_t>(file, width, height, FrameConstants::HAS_TIMESTAMP));
            //else throw FrameBPPTooBig(bpp);

            if (i % 10 == 0) { // TODO constant (Hay más)
                emit taskUpdate(TAG_LOAD, (i + 1.0f) / nframes);
            }
        }

        sampleFreq = nframes / ((frames.back().getTimestamp() - frames.first().getTimestamp()) * 1e-9);
        emit taskComplete(TAG_LOAD);

    }, [=](){
        file->close();
        delete file;
    }, [=](QString err){
        emit fileError(err);
    });
}

void Experiment::generateBasalFrame(uint32_t msStart, uint32_t msEnd)
{
    if (frames.isEmpty()) {
        basal = Frame<double>();
        return;
    }
    emit taskStart(TAG_BASALGEN);
    TaskLauncher::create([=](){
        int firstIndex = getFrameAt(msStart);
        int lastIndex  = getFrameAt(msEnd);
        int sz = lastIndex - firstIndex;
        Frame<double> top;
        Frame<double> bottom;
        Frame<double> aux = Frame<double>(width, height);

        for (int i = 0; i < sz; ++i) {
            aux += this->frames[i + firstIndex].cast<double>();

            if (i % 10 == 0) { // TODO constant (Hay más)
                emit taskUpdate(TAG_BASALGEN, (i + 1.0f) / sz);
            }
        }
        aux /= (lastIndex - firstIndex);
        aux = (aux - this->dark) * this->gain;
        aux.verticalSplit(height / 2, top, bottom);
        this->basal = hSaturation(top, bottom);
        emit basalFrame(this->basal.toQVariantList(FrameConstants::COLUM_MAJOR),
                        this->basal.getWidth(),
                        this->basal.getHeight());
        emit taskComplete(TAG_BASALGEN);
    });
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

    emit satFrame(aux.toQVariantList(FrameConstants::COLUM_MAJOR),
                  aux.getWidth(),
                  aux.getHeight(),
                  index,
                  frames.size() - indexStart,
                  topMean,
                  bottomMean);
}

#include <Eigen/Core>
#include <QDebug>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include "frame.h"
using namespace std;
void Experiment::calculateAllSatValues(uint32_t msStart)
{
    QAtomicInt * waiting = new QAtomicInt(0);
    for (int THREADS = 1; THREADS < 9; ++THREADS) {
        qDebug() << "THREADS: " << THREADS;
        for (int k = 0; k < 10; ++k) {
            int first = getFrameAt(msStart);
            int length = frames.size() - first;
            QAtomicInt *elementsDone = new QAtomicInt(0);
            QVector<std::function<void (void)>> tasks;

            A.clear();
            B.clear();
            A.reserve(length);
            B.reserve(length);
            for (int i = 0; i < length; ++i) {
                A.push_back(0);
                B.push_back(0);
            }

            emit taskStart(TAG_PROCESS);
            chrono::steady_clock::time_point start = chrono::steady_clock::now();

            for (int th = 0; th < THREADS; ++th) {
                int start = length * th / THREADS;
                int end   = length * (th + 1) / THREADS;

                tasks.push_back([=](){
                    Frame<double> aux;
                    Frame<double> top;
                    Frame<double> bottom;
                    for (int i = start; i < end; ++i) {
                        aux = frames[i].cast<double>();
                        aux = (aux - dark) * gain;
                        aux.verticalSplit(height / 2, top, bottom);
                        maskOperation(top, bottom);
                        A[i] = top.getData().mean();
                        B[i] = bottom.getData().mean();
                        if (Task::execEach(i, 100)) {
                            (*elementsDone) += 100;
                            emit taskUpdate(TAG_PROCESS, *elementsDone / double(length));
                        }
                    }
                });
            }

            *waiting = 1;
            TaskLauncher::afterAll(tasks, [=](){
                chrono::steady_clock::time_point bothEnd = chrono::steady_clock::now();
                emit satValues(A.toQVariantList(), B.toQVariantList());
                emit taskComplete(TAG_PROCESS);
                delete elementsDone;
                qDebug() << chrono::duration_cast<chrono::milliseconds>(bothEnd - start).count();
                *waiting = 0;
            });

            while(*waiting != 0)  {
                QThread::msleep(100);
            }
        }
    }

//    static int THREADS = 1;
//    int first = getFrameAt(msStart);
//    int length = frames.size() - first;
//    QAtomicInt *elementsDone = new QAtomicInt(0);
//    QVector<std::function<void (void)>> tasks;
//    QVector<Signal>* meanListsA = new QVector<Signal>();
//    QVector<Signal>* meanListsB = new QVector<Signal>();

//    emit taskStart(TAG_PROCESS);

//    const int max = 7 + 3;
//    QVector<QVector<double>>* values = new QVector<QVector<double>>();
//    for (int th = 0; th < THREADS; ++th) {
//        meanListsA->push_back(Signal());
//        meanListsB->push_back(Signal());
//        meanListsA->back().reserve(length / THREADS);
//        meanListsB->back().reserve(length / THREADS);

//        values->push_back(QVector<double>());
//        for (int i = 0; i < max; ++i) (*values)[th].push_back(0);
//        tasks.push_back([=](){
//            Frame<double> aux;
//            Frame<double> top;
//            Frame<double> bottom;
//            int start = first + length * th / THREADS;
//            int end   = first + length * (th + 1) / THREADS;

//            QElapsedTimer a;
//            std::chrono::steady_clock::time_point begin;
//            double time[7] = {0};
//            a.start();
//            int z = 0;
//            for (int i = start; i < end; ++i) {
//                begin = std::chrono::steady_clock::now();
//                aux = frames[i].cast<double>();
//                time[0] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
//                begin = std::chrono::steady_clock::now();
//                aux = (aux - dark) * gain;
//                time[1] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
//                begin = std::chrono::steady_clock::now();
//                aux.verticalSplit(height / 2, top, bottom);
//                time[2] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
//                begin = std::chrono::steady_clock::now();
//                maskOperation(top, bottom);
//                time[3] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
//                begin = std::chrono::steady_clock::now();
//                (*meanListsA)[th].append(top.getData().mean());
//                time[4] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
//                begin = std::chrono::steady_clock::now();
//                (*meanListsB)[th].append(bottom.getData().mean());
//                time[5] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
//                begin = std::chrono::steady_clock::now();
//                if (i % 100 == 0) {
//                    (*elementsDone) += 100;
//                    emit taskUpdate(TAG_PROCESS, *elementsDone / double(length));
//                }
//                time[6] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
//                begin = std::chrono::steady_clock::now();
//            }
//            (*values)[th][0] = end - start;
//            (*values)[th][1] = a.elapsed();
//            for (int i = 0; i < max; ++i) {
//                (*values)[th][2 + i] = time[i] / (*values)[th][0];
//            }
//            qDebug() << z;
//        });
//    }

//    TaskLauncher::afterAll(tasks, [=](){
//        A.clear();
//        B.clear();
//        A.reserve(length);
//        B.reserve(length);
//        for (Signal& signal : *meanListsA) {
//            A.append(signal);
//        }
//        for (Signal& signal : *meanListsB) {
//            B.append(signal);
//        }
//        emit satValues(A.toQVariantList(), B.toQVariantList());
//        emit taskComplete(TAG_PROCESS);
//        delete elementsDone;
//        delete meanListsA;
//        delete meanListsB;

//        QVector<double> avg;
//        for (int i = 0; i < max; ++i) avg.push_back(0);
//        for (int i = 0; i < values->size(); ++i) {
//            for (int j = 0; j < max; ++j) avg[j] += (*values)[i][j];
//            qDebug() << "Thread" << i << "took" << (*values)[i][1] << "ms with size" << (*values)[i][0];
//        }
//        qDebug() << "AVG size:" << (avg[0] / values->size());
//        qDebug() << "AVG time:" << (avg[1] / values->size());
//        for (int i = 2; i < max; ++i) {
//            qDebug() << "AVG:" << (avg[i] / values->size());
//        }
//    });
}

void Experiment::exportSatValuesToCSV(const QString &path, char separator)
{
    Signal::generateCSV(path, {&A, &B}, separator);
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


