
const BTN_DECIMALS = 3;
const BTN_MIN_WIDTH = 40;

const MODES = {
	DEFAULT: 1,
	ZOOM: 2
}

class OptionsBar {
  constructor(margins, onModeChange) {
		this.onModeChange = onModeChange;
		this.margins = margins;
		this.valueIndicator = new ValueIndicator();
		this.h = this.valueIndicator.h;
    this.zoomBtn = new Toggle("Zoom view");
		this.buttons = [this.valueIndicator, this.zoomBtn];
		this.setZoomMode(false);
  }

  updateButtonsWidth() {
    this.valueIndicator.updateWidth();
    this.zoomBtn.updateWidth();
  }

  addToStage(stage) {
    stage.addChild(this.valueIndicator);
    stage.addChild(this.zoomBtn);
  }

  draw(x, y) {
    x += this.margins;
    y -= this.margins;
		for (let i = 0; i < this.buttons.length; ++i) {
			let btn = this.buttons[i];
			if (btn.visible) {
				btn.draw(x, y - btn.h);
				x += btn.w + this.margins;
			}
		}
  }

	set hide(value) {
		if (value) {
			this.valueIndicator.visible = false;
	    this.zoomBtn.visible = false;
		} else {
			this.setZoomMode(this.mode); // This will reset visibility of buttons according to the mode
		}
	}

	getMode() {
		return this.mode;
	}

	setZoomMode(value) {
		if (value === OptionsBar.MODES.ZOOM) {
			this.mode = OptionsBar.MODES.ZOOM;
			this.valueIndicator.visible = true;
			this.zoomBtn.visible = true;
			this.zoomBtn.text = "Esc to exit";
		} else {
			this.mode = OptionsBar.MODES.DEFAULT;
			this.valueIndicator.visible = false;
			this.zoomBtn.visible = true;
			this.zoomBtn.text = "Zoom mode";
			console.log(this.zoomBtn.visible);
		}
		this.onModeChange(this.mode);
	}
}

class Toggle extends Button {
  constructor(text) {
    super(text);
    this.enabled = false;
  }
}

class ValueIndicator extends Button {
  constructor() {
    super("0", BTN_MIN_WIDTH);
  }

  set value(val) {
    this.text = val.toFixed(BTN_DECIMALS);
  }
}

OptionsBar.MODES = MODES;
