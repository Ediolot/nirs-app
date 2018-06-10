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

public:
    explicit WebInterface(QObject *parent = nullptr);
    ~WebInterface();

    Q_INVOKABLE void experimentFromFile(QString file);
    Q_INVOKABLE QString openFileDialog();

signals:
    void percentUpdateSignal(float);
    void fileErrorSignal(QString);
};

#endif // WEBINTERFACE_H
