
var webChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
    var JSobject = channel.objects.webinterface;
    JSobject.getInts(console.log);
    JSobject.getInts(console.log);
    JSobject.sayHello();
});
