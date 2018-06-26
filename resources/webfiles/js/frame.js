
const OPS = {
	NORMALIZE: 1,
	TRUNCATE: 2
}

class Frame {

	constructor(width, height, colormap) {
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
		this.sprite        = new PIXI.Sprite(PIXI.Texture.fromCanvas(this.canvas));
	}

	setImage(imgWidth, imgHeight, max, min, prefOperation, data) {
		this.max           = max;
		this.min           = min;
		this.prefOperation = prefOperation;
		this.data          = data;
		this.canvas.width  = imgWidth;
	  this.canvas.height = imgHeight;
	}

	fillCanvas() {
		if (this.data) {
			let w = this.canvas.width;
			let h = this.canvas.height;
			let img        = this.ctx.createImageData(w, h);
			let normFactor = 1.0 / (this.max - this.min);

			for (let i = 0, mx = h * w; i < mx; ++i) {
				let dst = i * 4;
				let val = this.data[i];

				if (this.prefOperation === Frame.OPS.NORMALIZE) {
					val = (val - this.min) * normFactor;
				}
				if (val > 1.0) val = 1.0
				else if (val < 0.0) val = 0.0;

				img.data[dst    ] = this.colormap.r(val) * 255.0;
				img.data[dst + 1] = this.colormap.g(val) * 255.0;
				img.data[dst + 2] = this.colormap.b(val) * 255.0;
				img.data[dst + 3] = 255;
			}
			this.ctx.putImageData(img, 0, 0);
		}
	}

	draw(x, y) {
		this.fillCanvas();
		this.sprite.scale.x = this.w / this.canvas.width;
		this.sprite.scale.y = this.h / this.canvas.height;
		this.sprite.x = x;
		this.sprite.y = y;
		this.sprite.texture.update();
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
	}

	get image() {
		return this.sprite;
	}
}

Frame.OPS = OPS;
