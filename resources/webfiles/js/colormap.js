
const CANVAS_WIDTH_PX = 1;
const CANVAS_HEIGHT_PX = 200;
const DECIMAL_PLACES = 2;
const LABELS_SEPARATION_PX = 5;

class Colormap {

	constructor(barWidth, height, fontSize, steps, max, min) {
		this.barWidth      = barWidth;
		this.h             = height;
		this.fontSize      = fontSize;
		this.steps         = steps;
		this.canvas        = document.createElement('canvas');
		this.canvas.width  = CANVAS_WIDTH_PX;
		this.canvas.height = CANVAS_HEIGHT_PX;
		this.ctx           = this.canvas.getContext('2d');
		this.sprite        = new PIXI.Sprite(PIXI.Texture.fromCanvas(this.canvas));
		this.max           = max;
		this.min           = min;
		this.labels        = [];
		this.genLabels();
	}

	addLabels(stage) {
		for (let i = 0; i < this.steps; ++i)
			stage.addChild(this.labels[i]);
	}

	setLabelAnchor(label, labelIndex) {
		label.anchor.y = (labelIndex == 0) ? 0 : (labelIndex < this.steps - 1) ? 0.5 : 1;
	}

	genLabels() {
		this.labels = [];
		for (let i = 0; i < this.steps; ++i) {
			let label = new PIXI.Text('', {fontSize: this.fontSize});
			this.setLabelAnchor(label, i);
			this.labels.push(label);
		}
	}

	updateLabelsText() {
		let step = (this.max - this.min) / (this.steps - 1);
		for (let i = 0; i < this.steps; ++i) {
			this.labels[i].text = (this.max - i * step).toFixed(DECIMAL_PLACES);
		}
	}

	placeLabels(x, y) {
		let step = this.h / (this.steps - 1);
		for (let i = 0; i < this.steps; ++i) {
			this.labels[i].x = x + this.barWidth + LABELS_SEPARATION_PX;
			this.labels[i].y = y + i * step;
		}
	}

	fillCanvas() {
		let w   = this.canvas.width;
		let h   = this.canvas.height;
		let img = this.ctx.createImageData(w, h);

		for (let i = 0; i < CANVAS_HEIGHT_PX; ++i) { // Width = 1
			let dst = i * 4;
			let val = (1.0 - i / CANVAS_HEIGHT_PX);
			img.data[dst    ] = this.r(val) * 255.0;
			img.data[dst + 1] = this.g(val) * 255.0;
			img.data[dst + 2] = this.b(val) * 255.0;
			img.data[dst + 3] = 255;
		}
		this.ctx.putImageData(img, 0, 0);
	}

	draw(x, y) {
		this.fillCanvas();
		this.sprite.scale.x = this.barWidth / this.canvas.width;
		this.sprite.scale.y = this.h        / this.canvas.height;
		this.sprite.x = x;
		this.sprite.y = y;
		this.placeLabels(x, y);
		this.sprite.texture.update();
	}

	resize(height) {
		this.h = height;
	}

	interpolate(val, y0, x0, y1, x1) {
    return (val - x0) * (y1 - y0) / (x1 - x0) + y0;
	}

	base(val) {
         if ( val <= -0.75 ) return 0;
    else if ( val <= -0.25 ) return this.interpolate(val, 0.0, -0.75, 1.0, -0.25);
    else if ( val <= +0.25 ) return 1.0;
    else if ( val <= +0.75 ) return this.interpolate(val, 1.0,  0.25, 0.0,  0.75);
    else return 0.0;
	}

	r(gray) {
    return this.base(gray * 2 - 1.5);
	}

	g(gray) {
    return this.base(gray * 2 - 1.0);
	}

	b(gray) {
    return this.base(gray * 2 - 0.5);
	}

	get image() {
		return this.sprite;
	}

	setMaxMin(max, min) {
		this.max = max;
		this.min = min;
		this.updateLabelsText();
	}

	set hide(value) {
		this.sprite.visible = !value;
		for (let i = 0; i < this.steps; ++i)
			this.labels[i].visible = !value;
	}
}
