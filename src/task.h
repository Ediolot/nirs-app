#ifndef TASK_H
#define TASK_H

#include <QThread>
#include <utility>
#include <functional>

class Task : public QThread
{
    Q_OBJECT
private:
    std::function<void (void)> task;
    std::function<void (void)> after;
    std::function<void (QString)> onError;

public:
    Task(std::function<void (void)> task, std::function<void (void)> after, std::function<void (QString)> onError);

protected:
    void run() override;
};

#endif // TASK_H
