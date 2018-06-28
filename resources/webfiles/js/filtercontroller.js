
const FILTERS = {
	BUTTER:			  "BUTTER",
	CHEBYSHEV_I:	"CHEBYSHEV_I",
	CHEBYSHEV_II: "CHEBYSHEV_II",
	LEGENDRE:			"LEGENDRE",
	BESSE:				"BESSE",
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
		element              = {};
		element.type         = $('#filter-type-' + name);
		element.order        = $('#filter-order-' + name);
		element.fc           = $('#filter-fc-' + name);
		element.fs           = $('#filter-fs-' + name);
		element.wildcardname = $('#filter-fs-wildcard-' + name + '-name');
		element.wildcard     = $('#filter-wildcard-' + name);
		element.ripple       = $('#filter-ripple-' + name);
		element.autoFs       = true;

		this.setupContenteditable(element.order, 1, 10, false);
		this.setupContenteditable(element.fc, 1, 99999, true);
		this.setupContenteditable(element.fs, 1, 99999, true);
		this.setupContenteditable(element.ripple, 0.1, 12, true);

		this.disable(element.fs);
		this.updateFilter(element);
		element.type.change(() => this.updateFilter(element));
		addCheckboxTriggerFunc('#filter-fs-checkauto-' + name, checked => {
			element.autoFs = checked;
			if (checked) {
				this.disable(element.fs);
			} else {
				this.enable(element.fs);
			}
		});
	}

	setupContenteditable(element, max, min, decimal) {
		element.keypress(function(e) {
			let number = Number(element.html());
			let char   = String.fromCharCode(e.which);
			if (isNaN(char) && !(char === '.' && decimal)) {
				e.preventDefault();
			}
			if (number < min) {
				element.html(min);
			}
			if (number > max) {
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
			this.setupContenteditable(filter.wildcard, 0.06, 16, true);
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
