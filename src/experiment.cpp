#include "experiment.h"

Experiment::Experiment()
    : QObject()
    , path("")
{
    assert(sizeof(float ) == 4); // 32 bit Float
    assert(sizeof(double) == 8); // 64 bit Double
}

Experiment::Experiment(const QString &path)
    : QObject()
    , path("")
{
    assert(sizeof(float ) == 4); // 32 bit Float
    assert(sizeof(double) == 8); // 64 bit Double
    load(path);
}

void Experiment::load(const QString &path)
{
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

        if (width > MAX_DIMENSION || height > MAX_DIMENSION) {
            throw FrameTooBigException(width, height);
        }

        // Frames
        dark = Frame<float>(*file, width, height, Superframe::Tstamp::DISABLED).cast<double>();
        gain = Frame<float>(*file, width, height, Superframe::Tstamp::DISABLED).cast<double>();

        int nframes = (file->size() - file->pos()) / (width * height * bpp / 8);
        frames.reserve(nframes);
        for (int i = 0; (i < nframes); ++i) {
                 if (bpp <=  8) frames.push_back(Frame< int8_t>(*file, width, height, Superframe::Tstamp::ENABLED).cast<double>());
            else if (bpp <= 16) frames.push_back(Frame<int16_t>(*file, width, height, Superframe::Tstamp::ENABLED).cast<double>());
            else if (bpp <= 32) frames.push_back(Frame<int32_t>(*file, width, height, Superframe::Tstamp::ENABLED).cast<double>());
            else if (bpp <= 64) frames.push_back(Frame<int64_t>(*file, width, height, Superframe::Tstamp::ENABLED).cast<double>());
            else throw FrameBPPTooBig(bpp);

            if (Task::execEach(i, UPDATE_INTERVAL)) {
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

void Experiment::generateBasalFrame(uint32_t start, uint32_t end, Type type)
{
    if (frames.isEmpty()) {
        basal = Frame<double>();
        return;
    }
    emit taskStart(TAG_BASALGEN);
    TaskLauncher::create([=](){
        int firstIndex = (type == MILLIS) ? getFrameAt(start) : start;
        int lastIndex  = (type == MILLIS) ? getFrameAt(end)   : end;
        int sz = lastIndex - firstIndex;
        Frame<double> top;
        Frame<double> bottom;
        Frame<double> aux = Frame<double>(width, height);

        for (int i = 0; i < sz; ++i) {
            aux += this->frames[i + firstIndex];

            if (Task::execEach(i, UPDATE_INTERVAL)) {
                emit taskUpdate(TAG_BASALGEN, (i + 1.0f) / sz);
            }
        }
        aux /= (lastIndex - firstIndex);
        aux = (aux - this->dark) * this->gain;
        aux.verticalSplit(height / 2, top, bottom);
        this->basal = hSaturation(top, bottom);
        emit basalFrame(this->basal.toQVariantList(Superframe::COL_MAJOR),
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
            if (m1(row, col) > MASK_VALUE || m2(row, col) > MASK_VALUE) {
                img1.set(row, col, 0);
                img2.set(row, col, 0);
            }
        }
    }
}

void Experiment::generateSatFrame(uint32_t pos, Type type)
{
    int index = (type == MILLIS) ? getFrameAt(pos) : pos;
    Frame<double> aux;
    Frame<double> top;
    Frame<double> bottom;
    double topMean;
    double bottomMean;

    aux = frames[index].cast<double>();
    aux = (aux - dark) * gain;
    aux.verticalSplit(height / 2, top, bottom);
    maskOperation(top, bottom);
    topMean = top.getData().mean();
    bottomMean = bottom.getData().mean();

    aux = hSaturation(top, bottom);
    aux -= basal;

    int max  = frames.size() - 1;
    int prev = (index > 0) ? index - 1 : 0;
    int next = (index < max) ? index + 1 : max;

    emit satFrame(aux.toQVariantList(Superframe::COL_MAJOR),
                  aux.getWidth(),
                  aux.getHeight(),
                  (type == MILLIS) ? getFrameRelativeTimeMS(prev) - 1 : prev, // The substraction of one unit allows for it to
                  (type == MILLIS) ? getFrameRelativeTimeMS(next) - 1 : next, // select the correct frame if rounding errors ocurr. (getFrameAt() selects the very next frame if the value is not exact)
                  (type == MILLIS) ? getFrameRelativeTimeMS(index) : index,
                  (type == MILLIS) ? getExperimentDurationMS() : frames.size(),
                  topMean,
                  bottomMean);
}

void Experiment::calculateAllSatValues(int roiX0, int roiY0, int roiX1, int roiY1, uint32_t start, Type type)
{
    int first = (type == MILLIS) ? getFrameAt(start) : start;
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

    int x0 = std::min(roiX0, roiX1);
    int y0 = std::min(roiY0, roiY1);
    int x1 = std::max(roiX0, roiX1);
    int y1 = std::max(roiY0, roiY1);

    emit taskStart(TAG_PROCESS);
    for (int th = 0; th < SAT_THREADS; ++th) {
        int indexStart = length * th / SAT_THREADS;
        int indexEnd   = length * (th + 1) / SAT_THREADS;

        tasks.push_back([=](){
            Frame<double> aux;
            Frame<double> top;
            Frame<double> bottom;
            for (int i = indexStart; i < indexEnd; ++i) {
                aux = frames[i].cast<double>();
                aux = (aux - dark) * gain;
                aux.verticalSplit(height / 2, top, bottom);
                maskOperation(top, bottom);
                A[i] = (x0 == x1 || y0 == y1) ? top.getData().mean() : top.getData().block(y0, x0, (y1 - y0), (x1 - x0)).mean();
                B[i] = (x0 == x1 || y0 == y1) ? bottom.getData().mean() : bottom.getData().block(y0, x0, (y1 - y0), (x1 - x0)).mean();
                if (Task::execEach(i, UPDATE_INTERVAL_LONG)) {
                    (*elementsDone) += UPDATE_INTERVAL_LONG;
                    emit taskUpdate(TAG_PROCESS, *elementsDone / double(length));
                }
            }
        });
    }

    TaskLauncher::afterAll(tasks, [=](){
        emit satValues(A.toQVariantList(), B.toQVariantList());
        emit taskComplete(TAG_PROCESS);
        delete elementsDone;
    });
}

void Experiment::exportSatValuesToCSV(const QString &path, char separator)
{
    Signal::generateCSV(path, {&A, &B}, separator);
}

uint64_t Experiment::getFrameRelativeTimeMS(int index) const
{
    return (frames[index].getTimestamp() - frames.front().getTimestamp()) / MS_TO_NS;
}

uint64_t Experiment::getExperimentDurationMS() const
{
    return (frames.back().getTimestamp() - frames.front().getTimestamp()) / MS_TO_NS;
}

const Frame<double> &Experiment::getBasal() const
{
    return basal;
}

uint64_t Experiment::frameToMs(uint64_t frame) const
{
    return (frames[frame].getTimestamp() - frames.front().getTimestamp()) / MS_TO_NS;
}

uint64_t Experiment::msToFrame(uint64_t ms) const
{
    return getFrameAt(ms);
}

qint64 Experiment::maxFrame() const
{
    return frames.size() - 1;
}

qint64 Experiment::maxMs() const
{
    return getExperimentDurationMS();
}

int Experiment::getStandardBpp(int bpp)
{
         if (bpp <=  8) return 8;
    else if (bpp <= 16) return 16;
    else if (bpp <= 32) return 32;
    else if (bpp <= 64) return 64;
    else                throw FrameBPPTooBig(bpp);
}

uint64_t Experiment::getFrameAt(uint64_t ms) const
{
    if (ms == 0) {
        return 0;
    }
    if (ms >= getExperimentDurationMS()) {
        return frames.size() - 1;
    }

    uint64_t ns = uint64_t(ms) * 1000000UL;
    uint64_t firstTimestamp = frames[0].getTimestamp();
    uint64_t currentNs = 0;
    int index = 0;

    while (currentNs < ns) {
        if (++index >= frames.size())
            throw BadIndexForFrameException(index, frames.size() - 1);

        currentNs = frames[index].getTimestamp() - firstTimestamp;
    }
    return index;
}

Frame<double> Experiment::hSaturation(Frame<double> img1, Frame<double> img2) const
{
    if ((img1.getWidth() != img2.getWidth()) || (img1.getHeight() != img2.getHeight()))
        throw SizeFrameMissmatch(img1.getWidth(), img1.getHeight(), img2.getWidth(), img2.getHeight());

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


