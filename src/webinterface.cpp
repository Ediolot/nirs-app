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
    exp->load(filepath);
}

QString WebInterface::openFileDialog()
{
    return QFileDialog::getOpenFileName(nullptr, "Open experiment", "", "Experiment (*.exp)");
}

void WebInterface::generateBasal(int end, QString type)
{
    current->generateBasalFrame(0, end, QStringToType(type));
}

void WebInterface::generateSatFrame(int pos, QString type)
{
    current->generateSatFrame(pos, QStringToType(type));
}

void WebInterface::calculateAllSatValues(int roiX0, int roiY0, int roiX1, int roiY1, int start, QString type)
{
    current->calculateAllSatValues(roiX0, roiY0, roiX1, roiY1, start, QStringToType(type));
}

void WebInterface::exportCSV(char separator)
{
    QString filename = QFileDialog::getSaveFileName(nullptr, "Export graph", "", "Comma separated values (*.csv)");
    if (filename.isEmpty()) {
        current->exportSatValuesToCSV(filename, separator);
    }
}

qint64 WebInterface::frameToMs(qint64 frame)
{
    return current->frameToMs(frame);
}

qint64 WebInterface::msToFrame(qint64 ms)
{
    return current->msToFrame(ms);
}

qint64 WebInterface::maxMs()
{
    return current->maxMs();
}

qint64 WebInterface::maxFrame()
{
    return current->maxFrame();
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

Experiment::Type WebInterface::QStringToType(const QString &str) const
{
    return (str == "MILLIS") ? Experiment::MILLIS : Experiment::FRAMES;
}
