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

void WebInterface::experimentFromFile(QString file)
{
    if (file.isEmpty()) {
        return;
    }

    Experiment* exp = new Experiment(file);
    experiments.push_back(exp);
    current = exp;
    connect(exp, &Experiment::loadPercent, [this](float value)   { emit percentUpdateSignal(value); });
    connect(exp, &Experiment::fileError,   [this](QString value) { emit fileErrorSignal(value);     });
    connect(exp, &Experiment::satValues,   [this](QVariantList A, QVariantList B) { emit satValues(A, B); });
    connect(exp, &Experiment::satFrame,    [this](QByteArray frame, int w, int h, int idx) { emit satFrameSignal(frame, w, h, idx); });
}

QString WebInterface::openFileDialog()
{
    return QFileDialog::getOpenFileName();
}

void WebInterface::generateBasal()
{
    current->calculateBasal(0, 24000);
    Frame<double> basal = current->getBasal();
    emit basalUpdateSignal(basal.toIndexed8Base64(FrameConstants::COLUM_MAJOR), basal.getWidth(), basal.getHeight());
}

void WebInterface::generateSatFrame(int index)
{
    current->generateSatFrame(index, 24000);
}

void WebInterface::calculateAllSatValues()
{
    current->calculateAllSatValues(24000);
}
