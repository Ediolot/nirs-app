#include "webinterface.h"

int WebInterface::getInts()
{
    return QDateTime::currentMSecsSinceEpoch() - QDateTime(QDate::currentDate()).toMSecsSinceEpoch();;
}

void WebInterface::sayHello()
{
    qDebug() << "Hello";
}

WebInterface::WebInterface(QObject *parent)
    : QObject(parent)
{

}
