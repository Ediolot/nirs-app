#include "webinterface.h"

WebInterface::WebInterface(QObject *parent)
    : QObject(parent)
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
    connect(exp, &Experiment::loadPercent, [this](float value)   { emit percentUpdateSignal(value); });
    connect(exp, &Experiment::fileError,   [this](QString value) { emit fileErrorSignal(value);     });
}

QString WebInterface::openFileDialog()
{
    return QFileDialog::getOpenFileName();
}
