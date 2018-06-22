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
    connect(exp, &Experiment::satFrame,    [this](QVariantList frame, int w, int h, int idx, int max) { emit   satFrameSignal(frame, w, h, idx, max); });
    connect(exp, &Experiment::basalFrame,  [this](QVariantList frame, int w, int h                  ) { emit basalFrameSignal(frame, w, h          ); });
    exp->load(filepath);
}

QString WebInterface::openFileDialog()
{
    return QFileDialog::getOpenFileName();
}

void WebInterface::generateBasal()
{
    current->generateBasalFrame(0, 24000);
}

void WebInterface::generateSatFrame(int index)
{
    current->generateSatFrame(index, 24000);
}

void WebInterface::calculateAllSatValues()
{
    current->calculateAllSatValues(24000);
}
