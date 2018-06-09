
$(document).ready(function() {
	$('#select-exp-button').click(e => {
		$('#select-exp-logic').click();
	});

	$('#select-exp-logic').change(e => {
	    $('#filepath-exp').html($(e.currentTarget).val());
	});

	$('#load-exp-button').click(function(e) {

	});

	let webChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
	    let JSobject = channel.objects.webinterface;
	    JSobject.getInts(console.log);
	    JSobject.getInts(console.log);
	    JSobject.sayHello();
	});
});
