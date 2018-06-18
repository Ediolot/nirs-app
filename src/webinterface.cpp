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
    connect(exp, &Experiment::satFrame,    [this](QByteArray frame, int w, int h, int idx) { emit satFrameSignal(frame, w, h, idx); });
    exp->load(filepath);
}

QString WebInterface::openFileDialog()
{
    return QFileDialog::getOpenFileName();
}

void WebInterface::generateBasal()
{
    current->calculateBasal(0, 1000);
    Frame<double> basal = current->getBasal();
    emit basalUpdateSignal(basal.toIndexed8Base64(FrameConstants::COLUM_MAJOR), basal.getWidth(), basal.getHeight());
}

void WebInterface::generateSatFrame(int index)
{
    current->generateSatFrame(index, 1000);
}

void WebInterface::calculateAllSatValues()
{
    current->calculateAllSatValues(1000);
}
