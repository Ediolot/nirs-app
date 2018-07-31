#include "webinterface.h"

WebInterface::WebInterface(QObject *parent)
    : QObject(parent)
    , current(nullptr)
{}

WebInterface::~WebInterface()
{
    for (Experiment *exp : experiments)
        delete exp;
}

void WebInterface::experimentFromFile(QString filepath)
{
    if (filepath.isEmpty()) {
        return;
    }

    Experiment* exp = new Experiment();
    experiments.push_back(exp);
    current = exp;
    connect(exp, &Experiment::taskComplete, [this](QString tag)             { emit taskCompleteSignal(tag);    });
    connect(exp, &Experiment::taskStart,    [this](QString tag)             { emit taskStartSignal(tag);       });
    connect(exp, &Experiment::taskUpdate,   [this](QString tag, double val) { emit taskUpdateSignal(tag, val); });

    connect(exp, &Experiment::fileError,   [this](QString value) { emit fileErrorSignal(value);     });
    connect(exp, &Experiment::satValues,   [this](QVariantList A, QVariantList B) { emit satValues(A, B); });
    connect(exp, &Experiment::satFrame,    [this](QVariantList frame, int w, int h, uint64_t prev, uint64_t next, uint64_t idx, uint64_t max) { emit satFrameSignal(frame, w, h, prev, next, idx, max); });
    connect(exp, &Experiment::basalFrame,  [this](QVariantList frame, int w, int h) { emit basalFrameSignal(frame, w, h); });
    connect(exp, &Experiment::previewFrame,[this](QVariantList frame, int w, int h) { emit previewFrameSignal(frame, w, h); });
    exp->load(filepath);
}

QString WebInterface::openFileDialog()
{
    return QFileDialog::getOpenFileName(nullptr, "Open experiment", "", "Experiment (*.exp)");
}

void WebInterface::generateBasal(int end, QString type)
{
    if (current) {
        current->generateBasalFrame(0, end, QStringToType(type));
    }
}

void WebInterface::generateSatFrame(int pos, QString type)
{
    if (current) {
        current->generateSatFrame(pos, QStringToType(type));
    }
}

void WebInterface::generatePreview(int kernelSize, double sigma)
{
    if (current) {
        current->generatePreviewBlurFrame(kernelSize, sigma);
    }
}

void WebInterface::calculateAllSatValues(int roiX0, int roiY0, int roiX1, int roiY1, int start, QString type)
{
    if (current) {
        current->calculateAllSatValues(roiX0, roiY0, roiX1, roiY1, start, QStringToType(type));
    }
}

void WebInterface::resetAllSatValues()
{
    if (current) {
        current->resetAllSatValues();
    }
}

void WebInterface::exportCSV(QString separator)
{
    QString filename = QFileDialog::getSaveFileName(nullptr, "Export graph", "", "Comma separated values (*.csv)");
    if (!filename.isEmpty() && current) {
        current->exportSatValuesToCSV(filename, separator.at(0).toLatin1());
    }
}

qint64 WebInterface::frameToMs(qint64 frame)
{
    return current ? current->frameToMs(frame) : 0;
}

qint64 WebInterface::msToFrame(qint64 ms)
{
    return current ? current->msToFrame(ms) : 0;
}

qint64 WebInterface::maxMs()
{
    return current ? current->maxMs() : 0;
}

qint64 WebInterface::maxFrame()
{
    return current ? current->maxFrame() : 0;
}

bool WebInterface::isExperimentLoaded()
{
    return current != nullptr;
}

void WebInterface::saveImageRGBA(QVariantList rgba, int width, int height)
{
    assert(width * height * 4 == rgba.size());
    QString filepath = QFileDialog::getSaveFileName(nullptr, "Export image", "", "Image (*.png)");
    QVector<uchar> pxData;
    QImage image;

    pxData.reserve(width * height * 4);
    for (const QVariant& value : rgba) {
        pxData.push_back(value.toInt());
    }
    image = QImage(pxData.data(), width, height, QImage::Format_RGBA8888);
    image.save(filepath);
}

void WebInterface::applySatFilter(QString name, QVariantList values)
{
    if (current) {
        current->applySatFilter(name, values);
    }
}

double WebInterface::requestSampleFreq()
{
    return current ? current->getSampleFreq() : DEF_SAMPLE_FREQ;
}

Experiment::Type WebInterface::QStringToType(const QString &str) const
{
    return (str == "MILLIS") ? Experiment::MILLIS : Experiment::FRAMES;
}
