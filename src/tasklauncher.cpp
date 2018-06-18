#include "tasklauncher.h"

void TaskLauncher::create(std::function<void ()> task, std::function<void ()> after, std::function<void (QString)> onError)
{
    Task *taskObj = new Task(std::move(task), std::move(after), std::move(onError));
    taskObj->start();
    connect(taskObj, SIGNAL(finished()), taskObj, SLOT(deleteLater()));
}

void TaskLauncher::create(std::function<void ()> task, std::function<void (QString)> onError)
{
    create(std::move(task), [](){}, std::move(onError));
}

void TaskLauncher::afterAll(QVector<std::function<void ()> > tasks, std::function<void ()> afterAll, std::function<void (QString)> onError)
{
    QAtomicInt* todo = new QAtomicInt(tasks.size());

    for (std::function<void ()>& task : tasks) {
        create(task, [=]() {
            if (--(*todo) == 0) {
                afterAll();
                delete todo;
            }
        }, onError);
    }
}
