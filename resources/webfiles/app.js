let navigatorId = 0;
let lineChart;
let label = 0;

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

		interface.taskCompleteSignal.connect(tag => {
      let element = null;
			if (tag === 'LOAD') {
        element = $('#load-exp-progress');
      } else if (tag === 'PROCESS') {
        element = $('#process-progress');
      }

      if (element) {
  			element.width('100%');
				element.hide();
      }
		});

		// QVariant(double, 0.625939) QVariant(double, 0.627675) QVariant(double, 0.627332) QVariant(double, 0.62529) QVariant(double, 0.627373) QVariant(double, 0.62532) QVariant(double, 0.627344) QVariant(double, 0.625778) QVariant(double, 0.627174) QVariant(double, 0.625323) QVariant(double, 0.627796) QVariant(double, 0.625486) QVariant(double, 0.628241) QVariant(double, 0.62466) QVariant(double, 0.627198)
		// QVariant(double, 0.625939) QVariant(double, 0.62529) QVariant(double, 0.62532) QVariant(double, 0.625778) QVariant(double, 0.625323) QVariant(double, 0.625486) QVariant(double, 0.62466) QVariant(double, 0.624901) QVariant(double, 0.62493) QVariant(double, 0.624232) QVariant(double, 0.625485) QVariant(double, 0.62421) QVariant(double, 0.626086) QVariant(double, 0.623704) QVariant(double, 0.625327)

		interface.taskStartSignal.connect(tag => {
      let element = null;
			if (tag === 'LOAD') {
        element = $('#load-exp-progress');
      } else if (tag === 'PROCESS') {
        element = $('#process-progress');
      }

      if (element) {
  			element.width('0%');
				element.show();
      }
		});

		interface.taskUpdateSignal.connect((tag, percent) => {
      let element = null;
			if (tag === 'LOAD') {
        element = $('#load-exp-progress');
      } else if (tag === 'PROCESS') {
        element = $('#process-progress');
      }

      if (element) {
        element.width(Math.round(100 * percent) + '%');
      }
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

		//-------------------------------------------

		$('#generate-graph-button').click(e => {
			interface.calculateAllSatValues();
		});

		//-------------------------------------------

		$('#navigator-prev').click(e => {
			if (navigatorId > 0) {
				interface.generateSatFrame(navigatorId - 1);
			}
		});

		$('#navigator-next').click(e => {
			interface.generateSatFrame(navigatorId + 1);
		});

		interface.satFrameSignal.connect((byteArray, width, height, index) => {
			fillCanvas('#sat-canvas', byteArray, width, height);
			$('#nav-number').html(index);
			navigatorId = index;
		});

		interface.satValues.connect((A, B) => {
		  let data = [];
		  for (let i = 0; i < A.length; ++i) {
		    data.push([i, A[i], B[i]]);
		  }
		  g.updateOptions( { 'file': data } );
		});
	});

  var g = new Dygraph(document.getElementById('graph'), [],
  {
    showRoller: true,
    labels: ['', 'Hhb', 'OxHb'],
		series: {
			Hhb: {
				color: '#2A8BB9',
				strokeWidth: 2,
				drawPoints: false
			},
			OxHb: {
				color: '#fc0023',
				strokeWidth: 2,
				drawPoints: false
			}
		},
		legend: 'always',
		gridLineColor: '#ddd'
  });
});

let fillCanvas = function(canvasId, byteArray, width, height) {
	let canvas = $(canvasId);
	let ctx = canvas[0].getContext('2d');
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
