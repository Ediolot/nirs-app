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

public:
    explicit WebInterface(QObject *parent = nullptr);
    ~WebInterface();

    Q_INVOKABLE void experimentFromFile(QString filepath);
    Q_INVOKABLE QString openFileDialog();
    Q_INVOKABLE void generateBasal(int end, QString type);
    Q_INVOKABLE void generateSatFrame(int pos, QString type);
    Q_INVOKABLE void calculateAllSatValues(int roiX0, int roiY0, int roiX1, int roiY1, int start, QString type);
    Q_INVOKABLE void exportCSV(char separator);
    Q_INVOKABLE qint64 frameToMs(qint64 frame);
    Q_INVOKABLE qint64 msToFrame(qint64 ms);
    Q_INVOKABLE qint64 maxMs();
    Q_INVOKABLE qint64 maxFrame();
    Q_INVOKABLE bool isExperimentLoaded();

signals:
    void taskUpdateSignal(QString, double);
    void taskCompleteSignal(QString);
    void taskStartSignal(QString);

    void fileErrorSignal(QString);
    void basalFrameSignal(QVariantList, int, int);
    void satFrameSignal(QVariantList, int, int, quint64, quint64, quint64, quint64);
    void satValues(QVariantList, QVariantList);

private:
    Experiment::Type QStringToType(const QString& str) const;
};

#endif // WEBINTERFACE_H
