
const SMOOTH_MAX = 9999;

let icons = {
  home:     null,
  settings: null,
  arduino:  null,
  about:    null,
	tests:    null,
  hp:       null,
  lp:       null,
  bp:       null,
	bs:       null
}

let sections = {
  home:     null,
  settings: null,
  arduino:  null,
  about:    null,
	tests:    null
}

let filters = {
  hp: null,
  lp: null,
  bp: null,
	bs: null
}

const BASAL_MILLIS = 24000;
const BASAL_FRAMES = 340;
let graph = null;
let graphData = [];
let navigatorId = 0;
let navigatorMax = 0;
let navigatorPrev = 0;
let navigatorNext = 0;
let basalEnd = BASAL_MILLIS;
let unitsType = "MILLIS";
let volume = 0.3;
let currentFilter;

$(document).ready( () => {
	loadPixi();
  icons.home        = $('#go-home');
  icons.settings    = $('#go-settings');
  icons.about       = $('#go-about');
  icons.arduino     = $('#go-arduino');
	icons.tests       = $('#go-tests');
	icons.hp          = $('#filter-hp-icon');
	icons.lp          = $('#filter-lp-icon');
	icons.bp          = $('#filter-bp-icon');
	icons.bs          = $('#filter-bs-icon');
  sections.home     = $('#home-section-cards');
  sections.settings = $('#settings-section');
  sections.about    = $('#about-section');
  sections.arduino  = $('#arduino-section');
	sections.tests    = $('#tests-section');
  sections.homeSmooth = $('#home-section-smooth');
	filters.hp        = $('#filter-hp');
	filters.lp        = $('#filter-lp');
	filters.bp        = $('#filter-bp');
	filters.bs        = $('#filter-bs');
	currentFilter = filters.hp;

	setupGraphSmoothListener();
	setupBasalNumListener();
	createGraph();
	addIconListeners();
	addCheckboxTrigger('#checkbox-invert', checked => updateGraph(invertHhR(checked, graphData)));
	addCheckboxTrigger('#checkbox-hbr',    checked => graph.setVisibility(0, checked));
	addCheckboxTrigger('#checkbox-hbo',    checked => graph.setVisibility(1, checked));

	onClick('#smooth-basal-button',  e => {
		moveSectionRight();
	});

	onClick('#go-back-icon', e => {
		moveSectionLeft();
	});

	onClick('#add-group', e => {
		let start = $('#start-group');
		let end   = $('#end-group');
		let row  = '<td><div class="circle"></div></td>';
			  row += '<td class="tests-list-text">START</td>';
			  row += '<td>' + start.val() + '</td>';
			  row += '<td class="tests-list-text">END</td>';
			  row += '<td>' + end.val() + '</td>';
			  row += '<td></td>';

		let startVal = Number(end.val()) + 1;
		let endVal = Number(end.val()) + 2;
		start.val(startVal);
		end.val(endVal);
		start.attr('min', startVal);
		end.attr('min', endVal);
		$('#groups-table tr:last').before('<tr>' + row + '</tr>');
	});

	onClick('#clear-group', e => {
		let start = $('#start-group');
		let end   = $('#end-group');
		let rows  = $('#groups-table tr').length;
		for (let i = 1; i < rows; ++i) {
			$('#groups-table tr:last').prev().remove();
		}
		start.val(340);
		end.val(341);
		start.attr('min', 340);
		end.attr('min', 341);
	})

	setQTInterface(qtInterface => {
		setupNavframeListener(qtInterface);
		changeUnitsType($('#unit-type').val(), qtInterface);
		let basalViewer      = new ImageViewer('#basal-viewer', qtInterface);
		let satViewer        = new ImageViewer('#sat-viewer', qtInterface);
		let smoothViewer     = new ImageViewer('#smooth-viewer', qtInterface);
		let filterController = new FilterController(qtInterface, addCheckboxTrigger);

		onClick('#apply-filter-button', e => {
			let filter = (currentFilter === filters.hp) ? filterController.hp
		             : (currentFilter === filters.lp) ? filterController.lp
				 	  	   : (currentFilter === filters.bp) ? filterController.bp : filterController.bs;

			qtInterface.applySatFilter(filter.name, filterController.getFilterData(filter));
		});

		$('#unit-type').change(() => {
			changeUnitsType($('#unit-type').val(), qtInterface);
		});

		onClick('#navigator-prev', e => {
			if (navigatorId > 0) {
				qtInterface.generateSatFrame(navigatorPrev, unitsType);
			}
		});

		onClick('#navigator-next', e => {
			if (navigatorId + 1 < navigatorMax) {
				qtInterface.generateSatFrame(navigatorNext, unitsType);
			}
		});

		onClick('#reset-graph-button', e => {
			qtInterface.resetAllSatValues();
		});

		onClick('#generate-graph-button', e => {
			let x0 = 0, y0 = 0, x1 = 0, y1 = 0;
			if (basalViewer.frame.roi.complete) {
				let roi    = basalViewer.frame.roi;
				let origin = basalViewer.frame.toOriginalCoord(roi.origin.x, roi.origin.y);
				let end    = basalViewer.frame.toOriginalCoord(roi.end.x, roi.end.y);
				x0 = origin.x;
				y0 = origin.y;
				x1 = end.x;
				y1 = end.y;
			}
			qtInterface.calculateAllSatValues(x0, y0, x1, y1, basalEnd, unitsType);
		});

		onClick('#graph-export', e => {
			qtInterface.exportCSV($('#csv-separator').html());
		});

		onClick('#select-exp-button', e => {
			qtInterface.openFileDialog(filepath => {
				$('#filepath-exp').val(filepath);
			});
		});

		onClick('#load-exp-button', e => {
			let filepath = $('#filepath-exp').val();
			if (filepath) {
				qtInterface.experimentFromFile(filepath);
				disableBtn('#load-exp-button');
			}
		});

		onClick('#generate-basal-button', e => {
			qtInterface.generateBasal(basalEnd, unitsType);
		});

		onClick('#preview-blur-button', e => {
			console.log($('#kernel-sigma').val() + " " + $('#kernel-size').val());
			qtInterface.generatePreview($('#kernel-size').val(), $('#kernel-sigma').val());
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
			play('audio/error.wav', volume);
			enableBtn('#load-exp-button');
		});

		qtInterface.basalFrameSignal.connect((data, width, height) => {
			let max = $('#basal-max').val();
			let min = $('#basal-min').val();
			let type = $('#basal-type').val() === 'NORMALIZE' ? Frame.OPS.NORMALIZE : Frame.OPS.TRUNCATE;
			basalViewer.setImage(width, height, max, min, type, data.splice(2));
		});

		qtInterface.previewFrameSignal.connect((data, width, height) => {
			let max = $('#sat-max').val();
			let min = $('#sat-min').val();
			let type = $('#sat-type').val() === 'NORMALIZE' ? Frame.OPS.NORMALIZE : Frame.OPS.TRUNCATE;
			smoothViewer.setImage(width, height, max, min, type, data.splice(2));
		});

		qtInterface.satFrameSignal.connect((data, width, height, prev, next, index) => {
			let max = $('#sat-max').val();
			let min = $('#sat-min').val();
			let type = $('#sat-type').val() === 'NORMALIZE' ? Frame.OPS.NORMALIZE : Frame.OPS.TRUNCATE;
			data = data.splice(2);
			satViewer.setImage(width, height, max, min, type, data);
			smoothViewer.setImage(width, height, max, min, type, data);
			navigatorId   = index;
			navigatorPrev = prev;
			navigatorNext = next;
			if (unitsType == "FRAMES") {
				qtInterface.maxFrame(val => { navigatorMax = val; updateInterfaceValues(); });
			} else {
				qtInterface.maxMs(val => { navigatorMax = val; updateInterfaceValues(); });
			}
			$('#start-group').attr('max', navigatorMax);
			$('#end-group').attr('max', navigatorMax);
		});

		// TASKS
		qtInterface.taskCompleteSignal.connect(tag => {
			let element = elementFromTag(tag);
			if (element) {
				element.width('100%');
				element.hide();
			}

			if (tag === 'LOAD') {
				enableBtn('#load-exp-button');
				enableBtn('#smooth-basal-button');
				enableBtn('#generate-basal-button');
				$('#load-error-text').html('');
				qtInterface.generateBasal(basalEnd, unitsType);
			}
			if (tag === 'PROCESS') {
				enableBtn('#reset-graph-button');
				enableBtn('#graph-export');
				enableBtn('#apply-filter-button');
				play('audio/ok.wav', volume);
			}
			if (tag === 'BASAL') {
				enableBtn('#navigator-prev');
				enableBtn('#navigator-next');
				enableBtn('#generate-graph-button');
				qtInterface.generateSatFrame(basalEnd, unitsType);
				play('audio/ok.wav', volume);
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

let disableBtn = function(id) {
	$(id).addClass('btn-disabled');
}

let enableBtn = function(id) {
	$(id).removeClass('btn-disabled');
}

let onClick = function(id, callback) {
	$(id).click(e => {
		if (!$(id).hasClass('btn-disabled')) {
			callback(e);
		}
	});
}

let play = function(file, volume) {
	let audio = new Audio(file);
	audio.volume = volume;
	audio.play();
}

let changeUnitsType = function(unit, qtInterface) {
	unitsType = unit;

	qtInterface.isExperimentLoaded(loaded => {
		if (loaded) {
			basalEnd = Number($('#basal-number').html());
			if (unitsType == "FRAMES") {
				qtInterface.msToFrame(basalEnd,      val => { basalEnd      = val; updateInterfaceValues() });
				qtInterface.msToFrame(navigatorId,   val => { navigatorId   = val; updateInterfaceValues() });
				qtInterface.msToFrame(navigatorPrev, val => { navigatorPrev = val; updateInterfaceValues() });
				qtInterface.msToFrame(navigatorNext, val => { navigatorNext = val; updateInterfaceValues() });
				qtInterface.maxFrame(                val => { navigatorMax  = val; updateInterfaceValues(); });
			} else {
				qtInterface.frameToMs(basalEnd,      val => { basalEnd      = val; updateInterfaceValues() });
				qtInterface.frameToMs(navigatorId,   val => { navigatorId   = val; updateInterfaceValues() });
				qtInterface.frameToMs(navigatorPrev, val => { navigatorPrev = val; updateInterfaceValues() });
				qtInterface.frameToMs(navigatorNext, val => { navigatorNext = val; updateInterfaceValues() });
				qtInterface.maxMs(                   val => { navigatorMax  = val; updateInterfaceValues(); });
			}
		} else {
			basalEnd = (unitsType == "MILLIS") ? BASAL_MILLIS : BASAL_FRAMES;
			updateInterfaceValues();
		}
	});
}

let updateInterfaceValues = function() {
	$('#basal-number').html(basalEnd);
	$('#basal-number-unit').html(unitsTypeAcronym());
	$('#nav-number').html(navigatorId);
	$('#nav-number-max').html("of " + navigatorMax + " " + unitsTypeAcronym());
}

let unitsTypeAcronym = function() {
	return (unitsType == "MILLIS") ? "ms" : "Frames";
}

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
			qtInterface.generateSatFrame(number, unitsType);
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
  if (section !== sections.tests   ) icons.tests   .removeClass('active'); else icons.tests   .addClass('active');

	moveSectionLeft(() => {
		let container = $('#body-content');
		container.stop();
	  container.animate({
	    scrollTop: section.offset().top - sections.home.offset().top
	  }, 800);
	});
}

let moveSectionLeft = function(callback) {
	let container = $('#body-content');
	if (container.scrollLeft() > 0) {
		container.stop();
		container.animate({
			scrollLeft: 0
		}, 800);
		setTimeout(callback, 800);
	} else {
		callback();
	}
}

let moveSectionRight = function() {
	let container = $('#body-content');
	container.stop();
	container.animate({
		scrollLeft: sections.homeSmooth.offset().left
	}, 800);
}

let goToFilter = function(filter) {
	if (filter !== filters.hp) icons.hp.removeClass('filter-active'); else icons.hp.addClass('filter-active');
	if (filter !== filters.lp) icons.lp.removeClass('filter-active'); else icons.lp.addClass('filter-active');
	if (filter !== filters.bp) icons.bp.removeClass('filter-active'); else icons.bp.addClass('filter-active');
	if (filter !== filters.bs) icons.bs.removeClass('filter-active'); else icons.bs.addClass('filter-active');

	currentFilter = filter;
	let container = $('#filters-scroll');
	container.stop();
  container.animate({
      scrollLeft: filter.offset().left - filters.hp.offset().left
  }, 400);
}

let addCheckboxTrigger = function(customCheckboxId, onChecked) {
	onClick(customCheckboxId, e => {
		let element = $(customCheckboxId + ' > .custom-checkbox-sq');
		if (element.hasClass('custom-checkbox-checked')) {
			onChecked(false);
		} else {
			onChecked(true);
		}
		element.toggleClass('custom-checkbox-unchecked');
		element.toggleClass('custom-checkbox-checked');
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
  icons.tests   .click(e => goToSection(sections.tests));
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

let setupBasalNumListener = function() {
	let basalNum = $("#basal-number");
	basalNum.keypress(function(e) {
		let number = Number(basalNum.html());
		if (isNaN(String.fromCharCode(e.which))) {
			e.preventDefault();
		}
		if (number < 1) {
			number = 1;
			basalNum.html(number);
		}
		if (e.which == 13) {
			e.preventDefault();
			basalEnd = number;
		}
	});

	basalNum.focusout(e => {
		basalNum.html(basalEnd);
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
