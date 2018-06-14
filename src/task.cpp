#include "task.h"

Task::Task(std::function<void ()> task, std::function<void (QString)> onError)
    : QThread()
    , task(std::move(task))
    , onError(std::move(onError))
{}

void Task::run()
{
    try {
        task();
    } catch (std::runtime_error &e) {
        onError(QString(e.what()));
    } catch (std::exception &e) {
        onError(QString(e.what()));
    }  catch (...) {
        onError(QString("Unkown error"));
    }
}

