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
    Frame<double> satFrame = current->generateSatFrame(index, 24000);
    emit satFrameSignal(satFrame.toIndexed8Base64(FrameConstants::COLUM_MAJOR), satFrame.getWidth(), satFrame.getHeight(), index);
}
