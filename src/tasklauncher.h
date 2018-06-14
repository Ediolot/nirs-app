#ifndef TASKLAUNCHER_H
#define TASKLAUNCHER_H

#include <QObject>
#include <utility>
#include "task.h"

class TaskLauncher : public QObject
{
    Q_OBJECT
private:
    Task *task;

public:
    TaskLauncher(std::function<void (void)> task, std::function<void (QString)> onError = [](QString dummy){});
};

#endif // TASKLAUNCHER_H
