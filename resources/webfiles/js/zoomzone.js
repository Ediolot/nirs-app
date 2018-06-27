
const CROSS_BORDER_PX = 1;
const CROSS_BORDER_COLOR = 0xFFFFFF;
const CROSS_ALPHA = 0.9;
const CROSS_SZ = 0.4;

class ZoomZone {
  constructor(zoneW, zoneH, scaleFactor, frame, margin) {
    this.margin        = margin;
    this.scaleFactor   = scaleFactor;
    this.frame         = frame;
    this.w             = zoneW;
    this.h             = zoneH;
    this.data          = null;
    this.imgW          = 0;
    this.imgH          = 0;
    this.zoomX         = 0;
    this.zoomY         = 0;
		this.canvas        = document.createElement('canvas');
		this.canvas.width  = this.w;
		this.canvas.height = this.h;
		this.ctx           = this.canvas.getContext('2d');
		this.sprite        = new PIXI.Sprite(PIXI.Texture.fromCanvas(this.canvas));
    this.cross         = new PIXI.Graphics();
    this.sprite.addChild(this.cross);
    this.cross.lineStyle(CROSS_BORDER_PX, CROSS_BORDER_COLOR, CROSS_ALPHA);
  }

  setCenter(x, y) {
    this.zoomX = x;
    this.zoomY = y;
  }

  fillCanvas() {
		if (this.frame.data) {
      let dw     = this.frame.canvas.width;
      let dh     = this.frame.canvas.height;
      let data   = this.frame.data;
			let w      = this.canvas.width;
			let h      = this.canvas.height;
      let frame  = this.frame;
			let img    = this.ctx.createImageData(w, h);
      let minRow = this.zoomY - (this.h * this.scaleFactor) / 2;
      let minCol = this.zoomX - (this.w * this.scaleFactor) / 2;

      for (let row = 0; row < w; ++row) {
        for (let col = 0; col < h; ++col) {
          let srcRow = minRow + Math.round(row * this.scaleFactor);
          let srcCol = minCol + Math.round(col * this.scaleFactor);
          let dst    = (col + row * w) * 4;
          let src    = (srcCol + srcRow * dw);
          let val;

          if (srcRow < 0 || srcRow >= dh || srcCol < 0 || srcCol >= dw) { // OUTSIDE DATA
    				img.data[dst    ] = img.data[dst + 1] = img.data[dst + 2] = 0;
            img.data[dst + 3] = 255;

          } else { // INSIDE DATA
            let val = data[src];
    				if (frame.prefOperation === Frame.OPS.NORMALIZE) {
    					val = (val - frame.min) * frame.normFactor;
    				}
    				if (val > 1.0) val = 1.0
    				else if (val < 0.0) val = 0.0;
    				else if (val === null || val === undefined) val = 0;

    				img.data[dst    ] = frame.colormap.r(val) * 255.0;
    				img.data[dst + 1] = frame.colormap.g(val) * 255.0;
    				img.data[dst + 2] = frame.colormap.b(val) * 255.0;
    				img.data[dst + 3] = 255;
          }
        }
      }
			this.ctx.putImageData(img, 0, 0);
		}
  }

  drawCross() {
    let w = this.w * CROSS_SZ;
    let h = this.h * CROSS_SZ;
    this.cross.moveTo(this.w / 2, this.h - h);
    this.cross.lineTo(this.w / 2, h);
    this.cross.moveTo(this.w - w, this.h / 2);
    this.cross.lineTo(w, this.h / 2);
  }

  draw(x, y, maxX, maxY) {
    this.fillCanvas();
		this.sprite.x = (x + this.margin + this.w) < maxX ? (x + this.margin) : (x - this.w - this.margin);
		this.sprite.y = (y + this.margin + this.h) < maxY ? (y + this.margin) : (y - this.h - this.margin);
    this.drawCross();
		this.sprite.texture.update();
  }

	get image() {
		return this.sprite;
	}

	set hide(value) {
		this.sprite.visible = !value;
	}
}
