#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include <QDebug>
#include <QObject>
#include <QDateTime>
#include <QFileDialog>
#include <QVector>

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

    Q_INVOKABLE void experimentFromFile(QString file);
    Q_INVOKABLE QString openFileDialog();
    Q_INVOKABLE void generateBasal();
    Q_INVOKABLE void generateSatFrame(int index);
    Q_INVOKABLE void generateAllSatFrames();

signals:
    void percentUpdateSignal(float);
    void fileErrorSignal(QString);
    void basalUpdateSignal(QByteArray, int, int);
    void satFrameSignal(QByteArray, int, int, int, double, double);
};

#endif // WEBINTERFACE_H
