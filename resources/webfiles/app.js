
$(document).ready(function() {
  let webChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
      let interface = channel.objects.webinterface;

			$('#select-exp-button').click(e => {
				interface.openFileDialog(filepath => {
					$('#filepath-exp').val(filepath);
				});
			});

      $('#load-exp-button').click(e => {
				let filepath = $('#filepath-exp').val();
				if (filepath) {
					interface.experimentFromFile(filepath);
				}
      });

			interface.percentUpdateSignal.connect(percent => {
				$('#load-exp-progress-val').width(Math.round(100 * percent) + "%");
			});

			interface.fileErrorSignal.connect(err => {
				console.error(err);
			});

			$('#generate-basal-button').click(e => {
				interface.generateBasal();
			});

			interface.basalUpdateSignal.connect((byteArray, width, height) => {
				fillCanvas('#basal-canvas', byteArray, width, height);
			});
	});
});

let fillCanvas = function(canvasId, byteArray, width, height) {
	let canvas = $(canvasId);
	let ctx = canvas[0].getContext("2d");
	let id = ctx.createImageData(width, height);
	let sz = width * height;

	let dec = atob(byteArray);
	let src = 0;
	let dst = 0;
	for (let i = 0; i < sz; ++i) {
		id.data[dst    ]   = red(dec.charCodeAt(src) / 128.0 - 1) * 255.0;
		id.data[dst + 1]   = green(dec.charCodeAt(src) / 128.0 - 1) * 255.0;
		id.data[dst + 2]   = blue(dec.charCodeAt(src) / 128.0 - 1) * 255.0;
		id.data[dst + 3]   = 255;
		dst += 4;
		src++;
	}
	ctx.putImageData(id, 0, 0);
}

let interpolate = function( val, y0, x0, y1, x1 ) {
    return (val-x0)*(y1-y0)/(x1-x0) + y0;
}

let base = function( val ) {
    if ( val <= -0.75 ) return 0;
    else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
    else if ( val <= 0.25 ) return 1.0;
    else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
    else return 0.0;
}

let red = function( gray ) {
    return base( gray - 0.5 );
}

let green = function( gray ) {
    return base( gray );
}

let blue = function( gray ) {
    return base( gray + 0.5 );
}
