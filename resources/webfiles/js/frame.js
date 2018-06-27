
const OPS = {
	NORMALIZE: 1,
	TRUNCATE: 2
}

const MODES = {
	DEFAULT: 1,
	ZOOMVIEW: 2
}

const OPS_MARGINS_PX = 5;
const ZOOM_ZONE_SZ_PX = 90;
const ZOOM_ZONE_FACTOR = 0.15;
const ZOOM_CURSOR_SPACE_PX = 10;

class Frame {

	constructor(width, height, colormap, onEnter, onExit, requestDraw) {
		this.roi           = null;
		this.requestDraw   = requestDraw;
		this.mode          = Frame.MODES.DEFAULT;
		this.roi           = new ZoomRectangle();
		this.zoomViewer    = new ZoomZone(ZOOM_ZONE_SZ_PX, ZOOM_ZONE_SZ_PX, ZOOM_ZONE_FACTOR, this, ZOOM_CURSOR_SPACE_PX);
		this.options       = new OptionsBar(OPS_MARGINS_PX);
		this.optionsInTop  = false;
		this.onEnter       = onEnter;
		this.onExit        = onExit;
		this.crossX        = 0;
		this.crossY        = 0;
		this.x             = 0;
		this.y             = 0;
		this.colormap      = colormap;
		this.w             = width;
		this.h             = height;
		this.imgW          = null;
		this.imgH          = null;
		this.max           = null;
		this.min           = null;
		this.prefOperation = null;
		this.data          = null;
		this.canvas        = document.createElement('canvas');
		this.canvas.width  = this.w;
		this.canvas.height = this.h;
		this.ctx           = this.canvas.getContext('2d');
		this.sprite             = new PIXI.Sprite(PIXI.Texture.fromCanvas(this.canvas));
		this.sprite.interactive = true;
		this.sprite.hitArea     = new PIXI.Rectangle(0, 0, this.w, this.h);
		this.sprite.mousemove   = mouseData => { this.onMousemove(mouseData); };
		this.sprite.mouseover   = () => {
			this.onEnter();
			this.updateVisibility(this.visible);
		};
		this.sprite.mouseout    = () => {
			this.zoomViewer.visible = false;
			this.onExit();
			this.requestDraw();
		};
		this.sprite.mousedown   = mouseData => {
			if (this.mode === Frame.MODES.DEFAULT) {
				let x = mouseData.data.global.x;
				let y = mouseData.data.global.y;
				this.roi.start(x, y);
			}
		}
		this.sprite.mouseup   = () => {
			this.roi.done();
		}

		this.options.zoomViewBtn.action = () => {
			this.setMode(Frame.MODES.ZOOM);
		};

		this.options.clearBtn.action = () => {
			this.roi.clean();
		}

    document.addEventListener('keydown', e => {
			if (e.key === "Escape") {
				this.setMode(Frame.MODES.DEFAULT);
			}
		});
	}

	setMode(mode) {
		this.mode = mode;
		this.updateVisibility(this.visible);
	}

	addTo(component) {
    component.addChild(this.sprite);
		this.options.addTo(component);
		this.roi.addTo(component);
		this.zoomViewer.addTo(component);
	}

	toOriginalCoord(x, y) {
		return {
			x: Math.round((x - this.x) / (this.w / this.canvas.width)),
			y: Math.round((y - this.y) / (this.h / this.canvas.height))
		}
	}

	onMousemove(mouseData) {
		if (this.data) {
			let x = mouseData.data.global.x;
			let y = mouseData.data.global.y;
			let sx = x - this.x;
			let sy = y - this.y;

			if (sx < this.w && sy < this.h)  {
				let o   = this.toOriginalCoord(x, y);
				let val = this.data[o.x + o.y * this.canvas.width];

				if (val) this.options.valueIndicator.value = val;
				this.roi.move(x, y);
				this.zoomViewer.setCenter(o.x, o.y);
				this.crossX = x;
				this.crossY = y;
				this.requestDraw();
			}

			this.optionsInTop = (this.mode == Frame.MODES.ZOOM)
			                &&  ((sy > (this.h - this.options.h)));

			// TODO THIS IS A TEMPORAL FIX
			if (sx > this.w || sy > this.h) {
				this.optionsInTop = false;
				this.onExit();
			} else {
				this.onEnter();
			}
		}
	}

	setImage(imgWidth, imgHeight, max, min, prefOperation, data) {
		this.max            = max;
		this.min            = min;
		this.prefOperation  = prefOperation;
		this.data           = data;
		this.canvas.width   = imgWidth;
	  this.canvas.height  = imgHeight;
		this.normFactor     = 1.0 / (max - min);
	}

	fillCanvas() {
		if (this.data) {
			let w   = this.canvas.width;
			let h   = this.canvas.height;
			let img = this.ctx.createImageData(w, h);

			for (let i = 0, mx = h * w; i < mx; ++i) {
				let dst = i * 4;
				let val = this.data[i];

				if (this.prefOperation === Frame.OPS.NORMALIZE) {
					val = (val - this.min) * this.normFactor;
				}
				if (val > 1.0) val = 1.0
				else if (val < 0.0) val = 0.0;
				else if (val === null || val === undefined) val = 0;

				img.data[dst    ] = this.colormap.r(val) * 255.0;
				img.data[dst + 1] = this.colormap.g(val) * 255.0;
				img.data[dst + 2] = this.colormap.b(val) * 255.0;
				img.data[dst + 3] = 255;
			}
			this.ctx.putImageData(img, 0, 0);
		}
	}

	draw(x, y) {
		this.x = x;
		this.y = y;
		this.fillCanvas();
		this.sprite.scale.x = this.w / this.canvas.width;
		this.sprite.scale.y = this.h / this.canvas.height;
		this.sprite.x = x;
		this.sprite.y = y;
		this.options.draw(x, this.optionsInTop ? (y + this.options.h + OPS_MARGINS_PX * 2) : (y + this.h));
		this.sprite.texture.update();
		this.sprite.texture.baseTexture.scaleMode = PIXI.SCALE_MODES.NEAREST;
		this.zoomViewer.draw(this.crossX, this.crossY, this.w, this.h);
		this.roi.draw();
	}

	resize(maxW, maxH, ratio) {
		if (!ratio) {
			ratio = maxW / maxH;
		}
		this.w = Math.round(maxW);
		this.h = Math.round(maxW / ratio);
		if (this.h > maxH) {
			this.w = Math.round(maxH * ratio);
			this.h = Math.round(maxH);
		}
		this.sprite.hitArea.width  = this.w;
		this.sprite.hitArea.height = this.h;
		this.roi.clean();
	}

	get image() {
		return this.sprite;
	}

	get visible() {
		return this.sprite.visible;
	}

	set visible(value) {
		this.updateVisibility(value);
	}

	updateVisibility(value) {
		this.sprite.visible  = value;
		if (this.mode == Frame.MODES.DEFAULT) {
			this.roi.disabled = false;
			this.options.zoomViewBtn.visible    = value;
			this.options.saveBtn.visible        = value;
			this.options.clearBtn.visible       = value;
			this.options.zoomMsg.visible        = false;
			this.options.valueIndicator.visible = false;
			this.zoomViewer.visible             = false;
		}
		else { // if (this.mode == Frame.MODES.ZOOM) {
			this.roi.disabled = true;
			this.options.zoomViewBtn.visible    = false;
			this.options.saveBtn.visible        = false;
			this.options.clearBtn.visible       = false;
			this.options.zoomMsg.visible        = value;
			this.options.valueIndicator.visible = value;
			this.zoomViewer.visible             = value;
		}
	}
}

Frame.OPS = OPS;
Frame.MODES = MODES;
