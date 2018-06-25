let navigatorId = 0;
let navigatorMax = 0;
let lineChart;
let label = 0;
let graphData = [];

const CANVAS_COLORMAP_WIDTH_PX = 4;
const CANVAS_COLORMAP_SPACE_PX = 5;
const CANVAS_COLORMAP_TEXT_PX = 22;
const CANVAS_COLORMAP_TEXT_INDICATORS = 4;
const CANVAS_COLORMAP_FONTSIZE = 20;
const TRUNCATE = 1;
const NORMALIZE = 2;
const RATIO = 260 / 172;
const SMOOTH_MAX = 9999;

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

let filterIcons = {
  hp: null,
  lp: null,
  bp: null,
	bs: null
}

let filters = {
  hp: null,
  lp: null,
  bp: null,
	bs: null
}

let canvasData = {}
let g = null;

$(document).ready(function() {

  icons.home        = $('#go-home');
  icons.settings    = $('#go-settings');
  icons.about       = $('#go-about');
  icons.arduino     = $('#go-arduino');
  sections.home     = $('#home-section');
  sections.settings = $('#settings-section');
  sections.about    = $('#about-section');
  sections.arduino  = $('#arduino-section');
	filters.hp        = $('#filter-hp');
	filters.lp        = $('#filter-lp');
	filters.bp        = $('#filter-bp');
	filters.bs        = $('#filter-bs');
	filterIcons.hp    = $('#filter-hp-icon');
	filterIcons.lp    = $('#filter-lp-icon');
	filterIcons.bp    = $('#filter-bp-icon');
	filterIcons.bs    = $('#filter-bs-icon');

  icons.home    .click(e => goToSection(sections.home));
  icons.settings.click(e => goToSection(sections.settings));
  icons.about   .click(e => goToSection(sections.about));
  icons.arduino .click(e => goToSection(sections.arduino));
  filterIcons.hp.click(e => goToFilter(filters.hp));
  filterIcons.lp.click(e => goToFilter(filters.lp));
  filterIcons.bp.click(e => goToFilter(filters.bp));
  filterIcons.bs.click(e => goToFilter(filters.bs));

	let graphAvg = $("#graph-avg");
	graphAvg.keypress(function(e) {
		let number = Number(graphAvg.html());
		if (isNaN(String.fromCharCode(e.which))) {
			e.preventDefault();
		}
		if (number >= SMOOTH_MAX) {
			number = SMOOTH_MAX;
			graphAvg.html(number);
			e.preventDefault();
		}
		if (number < 0) {
			number = 0;
			graphAvg.html(number);
		}
		if (e.which == 13) {
			e.preventDefault();
			$('.dygraph-roller').val(number);
			$('.dygraph-roller').trigger('change');
		}
	});

  let webChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
    let interface = channel.objects.webinterface;

		$('#select-exp-button').click(e => {
			interface.openFileDialog(filepath => {
				$('#filepath-exp').val(filepath);
			});
		});

		$('#graph-export').click(e => {
			interface.exportCSV(",");
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
				$('#load-error-text').html('');
				interface.generateBasal();
			}
			if (tag === 'BASAL') {
				interface.generateSatFrame(0);
			}
		});

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
			$('#load-error-text').html('Unable to load file.');
			$('#load-error-text').prop('title', err);
			$('#load-exp-progress').hide();
			$('#load-exp-progress').width('0%');
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
				id: '#basal-canvas',
				colormap: '#basal-colormap'
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
			if (navigatorId + 1 < navigatorMax) {
				interface.generateSatFrame(navigatorId + 1);
			}
		});

		interface.satFrameSignal.connect((data, width, height, index, max) => {
			canvasData.sat = {
				width: width,
				height: height,
				px: data.splice(2),
				operation: NORMALIZE,
				max: 0.06,
				min: 0.0,
				id: '#sat-canvas',
				colormap: '#sat-colormap'
			}
			fillCanvas(canvasData.sat);
			$('#nav-number').html(index);
			$('#nav-number-max').html(max - 1);
			navigatorId = index;
			navigatorMax = max;
		});

		interface.satValues.connect((A, B) => {
			let mx = A.length;
		  let data = new Array(mx);
		  for (let i = 0; i < mx; ++i) {
		    data[i] = [i, A[i], B[i]];
		  }
			updateGraph(data);
		});

		let navNumber = $("#nav-number");
		navNumber.keypress(function(e) {
			let number = Number(navNumber.html());
	    if (isNaN(String.fromCharCode(e.which))) {
				e.preventDefault();
			}
			if (number >= navigatorMax) {
				number = navigatorMax;
				navNumber.html(number);
			}
			if (number < 0) {
				number = 0;
				navNumber.html(number);
			}
			if (e.which == 13) {
				e.preventDefault();
				interface.generateSatFrame(number);
			}
		});

		navNumber.focusout(e => {
			navNumber.html(navigatorId);
		});
	});

	$(window).on('resize', function() {
		redrawAllCanvas();
	});

  g = new Dygraph(document.getElementById('graph'), [[0, 0, 0]],
  {
    showRoller: true,
    labels: ['', 'HbR', 'HbO'],
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

	redrawAllCanvas();

	addCheckboxTrigger('#checkbox-invert', checked => updateGraph(invertHhR(graphData)));
	addCheckboxTrigger('#checkbox-hbr',    checked => g.setVisibility(0, checked));
	addCheckboxTrigger('#checkbox-hbo',    checked => g.setVisibility(1, checked));
});

let invertHhR = function(data) {
	let mx = data.length;
	let inverted = new Array(mx);
	for (let i = 0; i < mx; ++i) {
		inverted[i] = data[i];
		inverted[i][2] = (-1) * inverted[i][2];
	}
	return inverted;
}

let updateGraph = function(data) {
	graphData = data;
	g.updateOptions( { 'file': data } );
}

let addCheckboxTrigger = function(customCheckboxId, onChecked) {
	$(customCheckboxId).click(e => {
		let element = $(customCheckboxId + ' > .custom-checkbox-sq');
		if (element.hasClass('custom-checbox-checked')) {
			onChecked(false);
		} else {
			onChecked(true);
		}
		element.toggleClass('custom-checbox-unchecked');
		element.toggleClass('custom-checbox-checked');
	});
}

let redrawAllCanvas = function() {
	if (canvasData.basal) fillCanvas(canvasData.basal);
	if (canvasData.sat)   fillCanvas(canvasData.sat);
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

		let container = $('#body-content');
		container.stop();
    container.animate({
        scrollTop: section.offset().top - sections.home.offset().top
    }, 800);
}

let goToFilter = function(filter) {
	if (filter !== filters.hp) filterIcons.hp.removeClass('filter-active'); else filterIcons.hp.addClass('filter-active');
	if (filter !== filters.lp) filterIcons.lp.removeClass('filter-active'); else filterIcons.lp.addClass('filter-active');
	if (filter !== filters.bp) filterIcons.bp.removeClass('filter-active'); else filterIcons.bp.addClass('filter-active');
	if (filter !== filters.bs) filterIcons.bs.removeClass('filter-active'); else filterIcons.bs.addClass('filter-active');

	let container = $('#filters-scroll');
	container.stop();
  container.animate({
      scrollLeft: filter.offset().left - filters.hp.offset().left
  }, 400);
}

let adjustToRatio = function(w, h, ratio) {
	let newW = w;
	let newH = w / ratio;
	if (newH > h) {
		newH = h;
		newW = h * ratio;
	}
	return {w: Math.floor(newW), h: Math.floor(newH)};
}

let fillCanvas = function(data) {
	let canvas     = $(data.id)[0];
	let ctx        = canvas.getContext('2d');
	canvas.width   = $(data.id).width();
	canvas.height  = $(data.id).height();
	let cmapW      = CANVAS_COLORMAP_WIDTH_PX + CANVAS_COLORMAP_SPACE_PX * 2 + CANVAS_COLORMAP_TEXT_PX;
	let availableW = canvas.width - cmapW;
	let availableH = canvas.height;
	let imgRatio   = data.width / data.height;
	let imgW       = adjustToRatio(availableW, availableH, imgRatio).w;
	let imgH       = adjustToRatio(availableW, availableH, imgRatio).h;
	let marginW    = (canvas.width - cmapW - imgW) / 2;
	let marginH    = (canvas.height - imgH) / 2;

	ctx.font.replace(/\d+px/, CANVAS_COLORMAP_FONTSIZE + "px");

	drawLabels(ctx,
						 marginW + imgW + CANVAS_COLORMAP_WIDTH_PX + CANVAS_COLORMAP_SPACE_PX * 2,
						 marginH,
						 imgH,
						 CANVAS_COLORMAP_TEXT_INDICATORS,
						 data);
	ctx.putImageData(drawColormapBar(ctx, CANVAS_COLORMAP_WIDTH_PX, imgH), marginW + imgW + CANVAS_COLORMAP_SPACE_PX, marginH);
	ctx.putImageData(drawImage(ctx, imgW, imgH, data), marginW, marginH);
}

let drawLabels = function(ctx, left, top, height, nlabels, data) {
	let labelStep  = height / (nlabels - 1);
  let lineHeight = ctx.measureText('M').width; // Approximation
	let valueMax   = (data.operation == NORMALIZE) ? data.max : 1.0;
	let valueStep  = ((data.operation == NORMALIZE) ? (data.max - data.min) : 1.0) / (nlabels - 1);
	for (let i = 0; i < nlabels; ++i) {
		let adjustY = (i == 0) ? lineHeight : (i == nlabels + 1) ? -lineHeight : 0;
		let y       = top + i * labelStep + adjustY;
		let val     = valueMax - i * valueStep;
		ctx.fillText(val.toFixed(2), left, y);
	}
}

let drawImage = function(ctx, w, h, data) {
	let img       = ctx.createImageData(w, h);
	let ratioCols = data.width / w;
	let ratioRows = data.height / h;
	let maxSrc    = data.width * data.height;
	let maxDst    = w * h * 4;

	for (let row = 0; row < h; ++row) {
		for (let col = 0; col < w; ++col) {
			let srcRow = (row * ratioRows) + 0.5 << 0; // Fast round
			let srcCol = (col * ratioCols) + 0.5 << 0; // Fast round
			let dst    = (col + row * w) * 4;
			let src    = (srcCol + srcRow * data.width);
			let val    = data.px[src];

			if (data.operation === NORMALIZE) {
				val = (val - data.min) / (data.max - data.min);
			}
			else if (val > 1.0) val = 1.0
			else if (val < 0.0) val = 0.0;

			img.data[dst    ] = red(  val * 2 - 1) * 255.0;
			img.data[dst + 1] = green(val * 2 - 1) * 255.0;
			img.data[dst + 2] = blue( val * 2 - 1) * 255.0;
			img.data[dst + 3] = 255;
		}
	}
	return img;
}

let drawColormapBar = function(ctx, w, h) {
	let colormap = ctx.createImageData(w, h);
	for (let row = 0; row < h; ++row) {
		for (let col = 0; col < w; ++col) {
			let dst = (col + row * w) * 4;
			let val = (1 - row / h);
			colormap.data[dst    ] = red(  val * 2 - 1) * 255.0;
			colormap.data[dst + 1] = green(val * 2 - 1) * 255.0;
			colormap.data[dst + 2] = blue( val * 2 - 1) * 255.0;
			colormap.data[dst + 3] = 255;
		}
	}
	return colormap;
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
