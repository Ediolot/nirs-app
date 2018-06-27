
const MARGINS_PX = 5;
const BTN_DECIMALS = 3;
const BTN_MIN_WIDTH = 40;

class OptionsBar {
  constructor() {
    this.valueIndicator = new ValueIndicator();
    this.zoomView = new Toggle("Zoom view");
    this.h = this.valueIndicator.h;
  }

  updateButtonsWidth() {
    this.valueIndicator.updateWidth();
    this.zoomView.updateWidth();
  }

  addToStage(stage) {
    stage.addChild(this.valueIndicator);
    stage.addChild(this.zoomView);
  }

  draw(x, y) {
    x += MARGINS_PX;
    y -= MARGINS_PX;
    this.valueIndicator.draw(x, y - this.valueIndicator.h);
    this.zoomView.draw(x + this.valueIndicator.w + MARGINS_PX, y - this.zoomView.h);
  }

	set hide(value) {
		this.valueIndicator.visible = !value;
    this.zoomView.visible = !value;
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
