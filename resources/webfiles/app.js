let navigatorId = 0;
let lineChart;
let label = 0;

const TRUNCATE = 1;
const NORMALIZE = 2;
const RATIO = 260 / 172;

let icons = {
  home:     null,
  settings: null,
  arduino:  null,
  about:    null
}

let sections = {
  home:     null,
  settings: null,
  arduino:  null,
  about:    null
}

let canvasData = {}

$(document).ready(function() {

  icons.home        = $('#go-home');
  icons.settings    = $('#go-settings');
  icons.about       = $('#go-about');
  icons.arduino     = $('#go-arduino');
  sections.home     = $('#home-section');
  sections.settings = $('#settings-section');
  sections.about    = $('#about-section');
  sections.arduino  = $('#arduino-section');

  icons.home    .click(e => goToSection(sections.home));
  icons.settings.click(e => goToSection(sections.settings));
  icons.about   .click(e => goToSection(sections.about));
  icons.arduino .click(e => goToSection(sections.arduino));

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
			let element = elementFromTag(tag);
      if (element) {
  		  element.width('100%');
			  element.hide();
      }

			if (tag === 'LOAD') {
				interface.generateBasal();
			}
			if (tag === 'BASAL') {
				interface.generateSatFrame(0);
			}
		});

		// QVariant(double, 0.625939) QVariant(double, 0.627675) QVariant(double, 0.627332) QVariant(double, 0.62529) QVariant(double, 0.627373) QVariant(double, 0.62532) QVariant(double, 0.627344) QVariant(double, 0.625778) QVariant(double, 0.627174) QVariant(double, 0.625323) QVariant(double, 0.627796) QVariant(double, 0.625486) QVariant(double, 0.628241) QVariant(double, 0.62466) QVariant(double, 0.627198)
		// QVariant(double, 0.625939) QVariant(double, 0.62529) QVariant(double, 0.62532) QVariant(double, 0.625778) QVariant(double, 0.625323) QVariant(double, 0.625486) QVariant(double, 0.62466) QVariant(double, 0.624901) QVariant(double, 0.62493) QVariant(double, 0.624232) QVariant(double, 0.625485) QVariant(double, 0.62421) QVariant(double, 0.626086) QVariant(double, 0.623704) QVariant(double, 0.625327)

		interface.taskStartSignal.connect(tag => {
			let element = elementFromTag(tag);
      if (element) {
  		  element.width('0%');
			  element.show();
      }
		});

		interface.taskUpdateSignal.connect((tag, percent) => {
			let element = elementFromTag(tag);
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

		interface.basalFrameSignal.connect((data, width, height) => {
			canvasData.basal = {
				width: width,
				height: height,
				px: data.splice(2),
				operation: TRUNCATE,
				max: 1.0,
				min: 0.0,
				id: '#basal-canvas'
			}
			fillCanvas(canvasData.basal);
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

		interface.satFrameSignal.connect((data, width, height, index) => {
			canvasData.sat = {
				width: width,
				height: height,
				px: data.splice(2),
				operation: NORMALIZE,
				max: 0.06,
				min: 0.0,
				id: '#sat-canvas'
			}
			fillCanvas(canvasData.sat);
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

	$(window).on('resize', function() {
		resizeAllCanvas();
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

	resizeAllCanvas();
});

let resizeAllCanvas = function() {
	resizeCanvas("#basal-canvas", "#basal-canvas-container", canvasData.basal);
	resizeCanvas("#sat-canvas", "#sat-canvas-container", canvasData.sat);
}

let resizeCanvas = function(canvasId, canvasContentId, canvasFillData) {
  let canvas = $(canvasId);
  let container = $(canvasContentId);
	let cw = container.width();
	let ch = container.height() - 60;
	let w  = cw;
	let h  = cw / RATIO;

	if (h > ch) {
		w = ch * RATIO;
		h = ch;
	}
	w = Math.round(w);
	h = Math.round(h);
	canvas.width(w);
	canvas.height(h);
	canvas[0].width = w;
	canvas[0].height = h;
	if (canvasFillData) {
		fillCanvas(canvasFillData);
	}
}

let elementFromTag = function(tag) {
	if (tag === 'LOAD') {
		return $('#load-exp-progress');
	} else if (tag === 'PROCESS') {
		return $('#graph-progress');
	} else if (tag === 'BASAL') {
		return $('#basal-progress');
	} else {
		return null;
	}
}

let goToSection = function(section) {
    if (section !== sections.home    ) icons.home    .removeClass('active'); else icons.home    .addClass('active');
    if (section !== sections.settings) icons.settings.removeClass('active'); else icons.settings.addClass('active');
    if (section !== sections.about   ) icons.about   .removeClass('active'); else icons.about   .addClass('active');
    if (section !== sections.arduino ) icons.arduino .removeClass('active'); else icons.arduino .addClass('active');

		let body = $('#body-content');
		body.stop();
    body.animate({
        scrollTop: section.offset().top - sections.home.offset().top
    }, 800);
}

let fillCanvas = function(data) {
	let canvas = $(data.id)[0];
	let ctx = canvas.getContext('2d');
	let id  = ctx.createImageData(canvas.width, canvas.height);
	let src = 0;
	let dst = 0;
	let ratioCols = data.width / canvas.width;
	let ratioRows = data.height / canvas.height;

	for (let row = 0; row < canvas.height; ++row) {
		for (let col = 0; col < canvas.width; ++col) {
			let srcRow = Math.round(row * ratioRows);
			let srcCol = Math.round(col * ratioCols);
			let dst    = (col + row * canvas.width) * 4;
			let src    = (srcCol + srcRow * data.width);
			let val    = data.px[src];

			if (data.operation === NORMALIZE) {
				val = (val - data.min) / (data.max - data.min);
			}
			if (val > 1.0) val = 1.0
			else if (val < 0.0) val = 0.0;

			id.data[dst    ]   = red(  val * 2 - 1) * 255.0;
			id.data[dst + 1]   = green(val * 2 - 1) * 255.0;
			id.data[dst + 2]   = blue( val * 2 - 1) * 255.0;
			id.data[dst + 3]   = 255;
		}
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
