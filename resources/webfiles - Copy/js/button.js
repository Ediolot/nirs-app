
const BTN_HEIGHT_PX = 16;
const BTN_WIDTH_PX = 50;
const BTN_BORDER_COLOR = 0xB5DAEE;
const BTN_COLOR = 0x3A5278;
const BTN_BORDER_PX = 2;
const BTN_ALPHA = 0.5;
const BTN_ROUND_PX = 4;
const BTN_TEXT_PADDING = 5;
const TEXT_STYLE = {fontSize: 12, fill: 0xFFFFFF};

class Button extends PIXI.Graphics {
  constructor(text, minW) {
    super();
    this.minW = minW;
    this.h = BTN_HEIGHT_PX;
    this.w = BTN_WIDTH_PX;
    this.font = new PIXI.Text('', TEXT_STYLE);
    this.font.x = this.w / 2;
    this.font.y = this.h / 2;
    this.font.resolution = 2;
    this.font.anchor.y = 0.5;
    this.font.anchor.x = 0.5;
    this.addChild(this.font);
    this.lineStyle(BTN_BORDER_PX, BTN_BORDER_COLOR, BTN_ALPHA);
    this.text = text;
  }

  updateWidth() {
    let w = PIXI.TextMetrics.measureText(this.text, new PIXI.TextStyle(TEXT_STYLE)).width;
    this.w = BTN_TEXT_PADDING * 2 + (w < this.minW ? this.minW : w);
    this.font.x = this.w / 2;
  }

  draw(x, y) {
    this.x = x;
    this.y = y;
    this.beginFill(BTN_COLOR, BTN_ALPHA);
    this.drawRoundedRect(0, 0, this.w, this.h, BTN_ROUND_PX);
    this.endFill();
  }

  set text(val) {
    this.font.text = val;
    this.updateWidth();
  }

  get text() {
    return this.font.text;
  }
}
