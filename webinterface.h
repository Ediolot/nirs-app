#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include <QWebEngineView>
#include <QString>

class WebInterface : public QWebEngineView
{
    Q_OBJECT
    const QString HTML_RESOURCE = "qrc:/webfiles/app.html";

public:
    explicit WebInterface(QWidget *parent = nullptr);
};

#endif // WEBINTERFACE_H
