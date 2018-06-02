#ifndef WEBENGINE_H
#define WEBENGINE_H

#include <QWebEngineView>
#include <QString>
#include <QWebChannel>

#include "webinterface.h"

class WebEngine : public QWebEngineView
{
    Q_OBJECT

private:
    const QString HTML_RESOURCE = "qrc:/webfiles/app.html";
    const QString WEB_INTERFACE_ID = "webinterface";

    WebInterface *interface;
    QWebChannel *channel;

public:
    explicit WebEngine(QWidget *parent = nullptr);
    ~WebEngine();
};

#endif // WEBINTERFACE_H
