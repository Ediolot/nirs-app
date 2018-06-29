
const FILTERS = {
	BUTTER:			  "BUTTER",
	CHEBYSHEV_I:	"CHEBYSHEV_I",
	CHEBYSHEV_II: "CHEBYSHEV_II",
	LEGENDRE:			"LEGENDRE",
	BESSEL:				"BESSEL",
	RBJ_BIQUAD:		"RBJ_BIQUAD",
	ELLIPTIC:			"ELLIPTIC"
}

class FilterController {
	constructor(qtInterface, addCheckboxTriggerFunc) {
		this.qtInterface = qtInterface;
		this.hp          = {};
		this.lp          = {};
		this.bp          = {};
		this.bs          = {};
		this.initFilter(this.hp, 'hp', addCheckboxTriggerFunc);
		this.initFilter(this.lp, 'lp', addCheckboxTriggerFunc);
		this.initFilter(this.bp, 'bp', addCheckboxTriggerFunc);
		this.initFilter(this.bs, 'bs', addCheckboxTriggerFunc);
	}

	initFilter(element, name, addCheckboxTriggerFunc) {
		element.name         = name;
		element.type         = $('#filter-type-' + name);
		element.order        = $('#filter-order-' + name);
		element.fc           = $('#filter-fc-' + name);
		element.fs           = $('#filter-fs-' + name);
		element.wildcardname = $('#filter-fs-wildcard-' + name + '-name');
		element.wildcard     = $('#filter-wildcard-' + name);
		element.ripple       = $('#filter-ripple-' + name);
		element.bwidth       = $('#filter-bandwidth-' + name);
		element.autoFs       = true;

		this.setupContenteditable(element.order, 1, 10, false);
		this.setupContenteditable(element.fc, 0, 99999, true);
		this.setupContenteditable(element.fs, 0, 99999, true);
		this.setupContenteditable(element.ripple, 0, 12, true);

		this.disable(element.fs);
		this.updateFilter(element);
		element.type.change(() => this.updateFilter(element));
		addCheckboxTriggerFunc('#filter-fs-checkauto-' + name, checked => {
			element.autoFs = checked;
			if (checked) {
				this.qtInterface.requestSampleFreq(val => { element.fs.html(val.toFixed(2)) });
				this.disable(element.fs);
			} else {
				this.enable(element.fs);
			}
		});
	}

	getFilterData(filter) {
		let type = filter.type.val();
		if (type === FILTERS.BUTTER)       return [type, filter.autoFs, Number(filter.order.html()), Number(filter.fc.html()), Number(filter.fs.html()),                            0,                             0 , Number(filter.bwidth.html())];
		if (type === FILTERS.CHEBYSHEV_I)  return [type, filter.autoFs, Number(filter.order.html()), Number(filter.fc.html()), Number(filter.fs.html()), Number(filter.ripple.html()),                             0 , Number(filter.bwidth.html())];
		if (type === FILTERS.CHEBYSHEV_II) return [type, filter.autoFs, Number(filter.order.html()), Number(filter.fc.html()), Number(filter.fs.html()),                            0, Number(filter.wildcard.html()), Number(filter.bwidth.html())];
		if (type === FILTERS.LEGENDRE)     return [type, filter.autoFs, Number(filter.order.html()), Number(filter.fc.html()), Number(filter.fs.html()),                            0,                             0 , Number(filter.bwidth.html())];
		if (type === FILTERS.BESSEL)       return [type, filter.autoFs, Number(filter.order.html()), Number(filter.fc.html()), Number(filter.fs.html()),                            0,                             0 , Number(filter.bwidth.html())];
		if (type === FILTERS.ELLIPTIC)     return [type, filter.autoFs, Number(filter.order.html()), Number(filter.fc.html()), Number(filter.fs.html()), Number(filter.ripple.html()), Number(filter.wildcard.html()), Number(filter.bwidth.html())];
		if (type === FILTERS.RBJ_BIQUAD)   return [type, filter.autoFs,                           0, Number(filter.fc.html()), Number(filter.fs.html()),                            0, Number(filter.wildcard.html()), Number(filter.bwidth.html())];
	}

	setupContenteditable(element, min, max, decimal) {
		element.keypress(function(e) {
			let number = Number(element.html());
			let char   = String.fromCharCode(e.which);
			if (isNaN(char) && !(char === '.' && decimal) && !(char === '-')) {
				e.preventDefault();
			}
			else if (number < min) {
				element.html(min);
			}
			else if (number > max) {
				element.html(max);
			}
		});
	}

	disable(element) {
		element.attr('contenteditable','false');
		element.parent().parent().addClass('row-disabled');
	}

	enable(element) {
		element.attr('contenteditable','true');
		element.parent().parent().removeClass('row-disabled');
	}

	updateFilters() {
		this.updateFilter(this.hp);
	}

	updateFilter(filter) {
		let selected = filter.type.val();
		if (selected === FILTERS.BUTTER) {
			this.disable(filter.wildcard);
			this.disable(filter.ripple);
		}
		else if (selected === FILTERS.CHEBYSHEV_I) {
			this.disable(filter.wildcard);
			this.enable(filter.ripple);
		}
		else if (selected === FILTERS.CHEBYSHEV_II) {
			this.enable(filter.wildcard);
			this.disable(filter.ripple);
			this.setupContenteditable(filter.wildcard, 3, 60, true);
			filter.wildcardname.html("Stop (dB)");
		}
		else if (selected === FILTERS.LEGENDRE) {
			this.disable(filter.wildcard);
			this.disable(filter.ripple);
		}
		else if (selected === FILTERS.BESSEL) {
			this.disable(filter.wildcard);
			this.disable(filter.ripple);
		}
		else if (selected === FILTERS.RBJ_BIQUAD) {
			this.enable(filter.wildcard);
			this.disable(filter.ripple);
			this.setupContenteditable(filter.wildcard, 0, 16, true);
			filter.wildcardname.html("Q");
		}
		else if (selected === FILTERS.ELLIPTIC) {
			this.enable(filter.wildcard);
			this.enable(filter.ripple);
			this.setupContenteditable(filter.wildcard, -16, 4, true);
			filter.wildcardname.html("W");
		}
	}
};

FilterController.FILTERS = FILTERS;
