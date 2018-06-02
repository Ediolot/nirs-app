#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include <QDebug>
#include <QObject>
#include <QDateTime>

class WebInterface : public QObject
{
    Q_OBJECT

private:
    int b = 0;
    int c() {return 0;}

public:
    int a = 0;
    int d() {return 0;}

    Q_INVOKABLE int getInts();
    Q_INVOKABLE void sayHello();

    explicit WebInterface(QObject *parent = nullptr);

signals:

public slots:
};

#endif // WEBINTERFACE_H
