let navigatorId = 0;
let lineChart;
let label = 0;

const TRUNCATE = 1;
const NORMALIZE = 2;

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
			fillCanvas('#basal-canvas', data.splice(2), width, height, TRUNCATE);
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
			fillCanvas('#sat-canvas', data.splice(2), width, height, NORMALIZE, 0, 0.06);
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

let fillCanvas = function(canvasId, data, width, height, operation, min, max) {
	let canvas = $(canvasId);
	let ctx = canvas[0].getContext('2d');
	let id = ctx.createImageData(width, height);
	let sz = width * height;
	let src = 0;
	let dst = 0;
	max = (max === undefined) ? 1.0 : max;
	min = (min === undefined) ? 0.0 : min;

	for (let i = 0; i < sz; ++i) {
		let val =  data[src];

		if (operation === NORMALIZE) {
			val = (val - min) / (max - min);
		}
		if (val > 1.0) val = 1.0
		else if (val < 0.0) val = 0.0;

		id.data[dst    ]   = red(  val * 2 - 1) * 255.0;
		id.data[dst + 1]   = green(val * 2 - 1) * 255.0;
		id.data[dst + 2]   = blue( val * 2 - 1) * 255.0;
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
