
const BTN_DECIMALS = 3;
const BTN_MIN_WIDTH = 40;

class OptionsBar {
  constructor(margins) {
		this.valueIndicator = new ValueIndicator();
    this.zoomViewBtn    = new Button("Zoom view");
		this.saveBtn        = new Button("Save");
		this.clearBtn       = new Button("Clear ROI");
    this.zoomMsg        = new Message("Esc to exit mode");
		this.margins        = margins;
		this.h              = this.valueIndicator.h;
		this.buttons = [
			this.valueIndicator, this.zoomViewBtn, this.zoomMsg, this.saveBtn,
			this.clearBtn
		];
  }

  updateButtonsWidth() {
		for (let i = 0; i < this.buttons.length; ++i)
			this.buttons[i].updateWidth();
  }

  addTo(component) {
		for (let i = 0; i < this.buttons.length; ++i)
			component.addChild(this.buttons[i]);
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
}

class ValueIndicator extends Message {
  constructor() {
    super("0", BTN_MIN_WIDTH);
  }

  set value(val) {
    this.text = val.toFixed(BTN_DECIMALS);
  }
}
