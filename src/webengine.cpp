#include "webengine.h"

WebEngine::WebEngine(QWidget *parent)
    : QWebEngineView(parent)
    , interface(new WebInterface(this))
    , channel(new QWebChannel(this))
{
    setUrl(QUrl(HTML_RESOURCE));

    channel->registerObject(WEB_INTERFACE_ID, interface);
    page()->setWebChannel(channel);
}

WebEngine::~WebEngine()
{
    delete interface;
    delete channel;
}



