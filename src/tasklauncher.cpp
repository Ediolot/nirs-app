#include "tasklauncher.h"

TaskLauncher::TaskLauncher(std::function<void ()> task, std::function<void (QString)> onError)
    : QObject()
    , task(new Task(std::move(task), std::move(onError)))
{
    this->task->start();
    connect(this->task, SIGNAL(finished()), this->task, SLOT(deleteLater()));
}
