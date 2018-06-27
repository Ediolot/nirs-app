
const SMOOTH_MAX = 9999;

let icons = {
  home:     null,
  settings: null,
  arduino:  null,
  about:    null,
  hp:       null,
  lp:       null,
  bp:       null,
	bs:       null
}

let sections = {
  home:     null,
  settings: null,
  arduino:  null,
  about:    null
}

let filters = {
  hp: null,
  lp: null,
  bp: null,
	bs: null
}

let graph = null;
let graphData = [];
let navigatorId = 0;
let navigatorMax = 0;


$(document).ready( () => {
	loadPixi();
	let basalViewer   = new ImageViewer('#basal-viewer', 1.0, 0.0);
	let satViewer     = new ImageViewer('#sat-viewer', 0.06, 0.0);
  icons.home        = $('#go-home');
  icons.settings    = $('#go-settings');
  icons.about       = $('#go-about');
  icons.arduino     = $('#go-arduino');
	icons.hp          = $('#filter-hp-icon');
	icons.lp          = $('#filter-lp-icon');
	icons.bp          = $('#filter-bp-icon');
	icons.bs          = $('#filter-bs-icon');
  sections.home     = $('#home-section');
  sections.settings = $('#settings-section');
  sections.about    = $('#about-section');
  sections.arduino  = $('#arduino-section');
	filters.hp        = $('#filter-hp');
	filters.lp        = $('#filter-lp');
	filters.bp        = $('#filter-bp');
	filters.bs        = $('#filter-bs');

	setupGraphSmoothListener();
	createGraph();
	addIconListeners();
	addCheckboxTrigger('#checkbox-invert', checked => updateGraph(invertHhR(checked, graphData)));
	addCheckboxTrigger('#checkbox-hbr',    checked => graph.setVisibility(0, checked));
	addCheckboxTrigger('#checkbox-hbo',    checked => graph.setVisibility(1, checked));

	setQTInterface(qtInterface => {
		setupNavframeListener(qtInterface);

		$('#navigator-prev').click(e => {
			if (navigatorId > 0) {
				qtInterface.generateSatFrame(navigatorId - 1);
			}
		});

		$('#navigator-next').click(e => {
			if (navigatorId + 1 < navigatorMax) {
				qtInterface.generateSatFrame(navigatorId + 1);
			}
		});

		$('#generate-graph-button').click(e => {
			qtInterface.calculateAllSatValues();
		});

		$('#graph-export').click(e => {
			qtInterface.exportCSV(",");
		});

		$('#select-exp-button').click(e => {
			qtInterface.openFileDialog(filepath => {
				$('#filepath-exp').val(filepath);
			});
		});

		$('#load-exp-button').click(e => {
			let filepath = $('#filepath-exp').val();
			if (filepath) {
				qtInterface.experimentFromFile(filepath);
			}
		});

		$('#generate-basal-button').click(e => {
			qtInterface.generateBasal();
		});

		qtInterface.satValues.connect((A, B) => {
			let mx = A.length;
		  let data = new Array(mx);
		  for (let i = 0; i < mx; ++i) {
		    data[i] = [i, A[i], B[i]];
		  }
			updateGraph(data);
		});

		qtInterface.fileErrorSignal.connect(err => {
			$('#load-error-text').html('Unable to load file.');
			$('#load-error-text').prop('title', err);
			$('#load-exp-progress').hide();
			$('#load-exp-progress').width('0%');
		});

		qtInterface.basalFrameSignal.connect((data, width, height) => {
			basalViewer.setImage(width, height, 1.0, 0.0, Frame.OPS.TRUNCATE, data.splice(2));
		});

		qtInterface.satFrameSignal.connect((data, width, height, index, max) => {
			satViewer.setImage(width, height, 0.06, 0.0, Frame.OPS.NORMALIZE, data.splice(2));
			$('#nav-number').html(index);
			$('#nav-number-max').html("of " + (max - 1));
			navigatorId = index;
			navigatorMax = max;
		});

		// TASKS
		qtInterface.taskCompleteSignal.connect(tag => {
			let element = elementFromTag(tag);
			if (element) {
				element.width('100%');
				element.hide();
			}

			if (tag === 'LOAD') {
				$('#load-error-text').html('');
				qtInterface.generateBasal();
			}
			if (tag === 'BASAL') {
				qtInterface.generateSatFrame(0);
			}
		});

		qtInterface.taskStartSignal.connect(tag => {
			let element = elementFromTag(tag);
			if (element) {
				element.width('0%');
				element.show();
			}
		});

		qtInterface.taskUpdateSignal.connect((tag, percent) => {
			let element = elementFromTag(tag);
			if (element) {
				element.width(Math.round(100 * percent) + '%');
			}
		});
	});
});

// ----------------------------------------------------------------------------

let setupNavframeListener = function(qtInterface) {
	let navNumber = $("#nav-number");
	navNumber.keypress(function(e) {
		let number = Number(navNumber.html());
    if (isNaN(String.fromCharCode(e.which))) {
			e.preventDefault();
		}
		if (number >= navigatorMax) {
			number = navigatorMax;
			navNumber.html(number);
			e.preventDefault();
		}
		if (number < 0) {
			number = 0;
			navNumber.html(number);
		}
		if (e.which == 13) {
			e.preventDefault();
			qtInterface.generateSatFrame(number);
		}
	});

	navNumber.focusout(e => {
		navNumber.html(navigatorId);
	});
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
	if (filter !== filters.hp) icons.hp.removeClass('filter-active'); else icons.hp.addClass('filter-active');
	if (filter !== filters.lp) icons.lp.removeClass('filter-active'); else icons.lp.addClass('filter-active');
	if (filter !== filters.bp) icons.bp.removeClass('filter-active'); else icons.bp.addClass('filter-active');
	if (filter !== filters.bs) icons.bs.removeClass('filter-active'); else icons.bs.addClass('filter-active');

	let container = $('#filters-scroll');
	container.stop();
  container.animate({
      scrollLeft: filter.offset().left - filters.hp.offset().left
  }, 400);
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

let invertHhR = function(abs, data) {
	let mx = data.length;
	let inverted = new Array(mx);
	for (let i = 0; i < mx; ++i) {
		inverted[i] = data[i];
		inverted[i][2] = (-1) * inverted[i][2];
	}
	return inverted;
}

let updateGraph = function(data) {
	if (data.length > 1) {
		$('.dygraph-rangesel-zoomhandle').css('opacity', '1');
		graphData = data;
		graph.updateOptions( { 'showRangeSelector': true, 'file': data } );
	} else {
		graph.updateOptions( { 'showRangeSelector': false } );
	}
}

let addIconListeners = function() {
  icons.home    .click(e => goToSection(sections.home));
  icons.settings.click(e => goToSection(sections.settings));
  icons.about   .click(e => goToSection(sections.about));
  icons.arduino .click(e => goToSection(sections.arduino));
  icons.hp      .click(e => goToFilter(filters.hp));
  icons.lp      .click(e => goToFilter(filters.lp));
  icons.bp      .click(e => goToFilter(filters.bp));
  icons.bs      .click(e => goToFilter(filters.bs));
}

let createGraph = function() {
	graph = new Dygraph(document.getElementById('graph'), [[0, 0, 0]],
	{
		xlabel: 'Frames',
		showRoller: true,
		showRangeSelector: false,
		labels: ['', 'HbR', 'HbO'],
		series: {
			HbR: {
				color: '#2A8BB9',
				strokeWidth: 2,
				drawPoints: false
			},
			HbO: {
				color: '#fc0023',
				strokeWidth: 2,
				drawPoints: false
			}
		},
		legend: 'always',
		gridLineColor: '#ddd'
	});
}

let setupGraphSmoothListener = function() {
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
}

let loadPixi = function() {
	PIXI.utils.sayHello(PIXI.utils.isWebGLSupported() ? "WebGL" : "canvas");
}

let setQTInterface = function(onQTInterface) {
  new QWebChannel(qt.webChannelTransport, function(channel) {
		onQTInterface(channel.objects.webinterface);
	});
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
