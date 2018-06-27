
const BTN_DECIMALS = 3;
const BTN_MIN_WIDTH = 40;

class OptionsBar {
  constructor(margins) {
		this.margins = margins;
		this.valueIndicator = new ValueIndicator();
		this.h = this.valueIndicator.h;
    this.zoomBtn = new Button("Zoom view");
		this.saveBtn = new Button("Save");
    this.zoomMsg = new Message("Esc to exit mode");
		this.buttons = [this.valueIndicator, this.zoomBtn, this.zoomMsg, this.saveBtn];
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
