#include "webinterface.h"

WebInterface::WebInterface(QWidget *parent)
    : QWebEngineView(parent)
{
    setUrl(QUrl(HTML_RESOURCE));
}
