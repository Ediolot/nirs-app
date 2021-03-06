#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include <QDebug>
#include <QObject>
#include <QDateTime>
#include <QFileDialog>
#include <QVector>
#include <QVariantList>

#include "experiment.h"

class WebInterface : public QObject
{
    Q_OBJECT

private:
    QVector<Experiment*> experiments;
    Experiment* current;

    const int DEF_SAMPLE_FREQ = 14;

public:
    explicit WebInterface(QObject *parent = nullptr);
    ~WebInterface();

    Q_INVOKABLE void experimentFromFile(QString filepath);
    Q_INVOKABLE QString openFileDialog();
    Q_INVOKABLE void generateBasal(int end, QString type);
    Q_INVOKABLE void generateSatFrame(int pos, QString type);
    Q_INVOKABLE void generatePreview(int kernelSize, double sigma);
    Q_INVOKABLE void calculateAllSatValues(int roiX0, int roiY0, int roiX1, int roiY1, int start, QString type);
    Q_INVOKABLE void resetAllSatValues();
    Q_INVOKABLE void exportCSV(QString separator);
    Q_INVOKABLE qint64 frameToMs(qint64 frame);
    Q_INVOKABLE qint64 msToFrame(qint64 ms);
    Q_INVOKABLE qint64 maxMs();
    Q_INVOKABLE qint64 maxFrame();
    Q_INVOKABLE bool isExperimentLoaded();
    Q_INVOKABLE void saveImageRGBA(QVariantList rgba, int width, int height);
    Q_INVOKABLE void applySatFilter(QString name, QVariantList values);
    Q_INVOKABLE double requestSampleFreq();

signals:
    void taskUpdateSignal(QString, double);
    void taskCompleteSignal(QString);
    void taskStartSignal(QString);

    void fileErrorSignal(QString);
    void basalFrameSignal(QVariantList, int, int);
    void previewFrameSignal(QVariantList, int, int);
    void satFrameSignal(QVariantList, int, int, quint64, quint64, quint64, quint64);
    void satValues(QVariantList, QVariantList);

private:
    Experiment::Type QStringToType(const QString& str) const;
};

#endif // WEBINTERFACE_H
