#ifndef TASKLAUNCHER_H
#define TASKLAUNCHER_H

#include <QObject>
#include <utility>
#include <QVector>
#include <QAtomicInt>
#include "task.h"

class TaskLauncher : public QObject
{
    Q_OBJECT

public:
    static void create(std::function<void (void)> task,
                       std::function<void (void)> after,
                       std::function<void (QString)> onError = [](QString dummy){});

    static void create(std::function<void (void)> task,
                       std::function<void (QString)> onError = [](QString dummy){});

    static void afterAll(QVector<std::function<void (void)>> tasks,
                         std::function<void (void)> afterAll,
                         std::function<void (QString)> onError = [](QString dummy){});
};

#endif // TASKLAUNCHER_H
