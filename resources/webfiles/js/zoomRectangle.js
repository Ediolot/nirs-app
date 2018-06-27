ZOOM_RECT_LINE_PX = 2;
ZOOM_RECT_COLOR = 0xFFFFFF;
ZOOM_RECT_ALPHA = 0.8;


class ZoomRectangle extends PIXI.Graphics {
	constructor() {
		super()
		this.origin = {};
		this.end    = {};
		this.end.x    = 0;
		this.end.y    = 0;
		this.origin.x = 0;
		this.origin.y = 0;
		this.drawing  = false;
		this.follow   = true;
		this.disabled = false;
		this.complete = false;
	}

	addTo(component) {
    component.addChild(this);
	}

	clean() {
		this.drawing = false;
		this.complete = false;
	}

	move(x, y) {
		if (this.drawing && this.follow) {
			this.end.x = x;
			this.end.y = y;
		}
	}

	start(x, y) {
		this.complete = false;
		this.follow   = true;
		this.drawing  = true;
		this.origin.x = this.end.x = x;
		this.origin.y = this.end.y = y;
	}

	done() {
		if (this.drawing) {
			this.follow = false;
			this.complete = true;
		}
	}

	draw() {
		this.clear();
		if (this.drawing && !this.disabled) {
	    this.lineStyle(ZOOM_RECT_LINE_PX, ZOOM_RECT_COLOR, ZOOM_RECT_ALPHA);
			this.moveTo(this.origin.x, this.origin.y);
			this.lineTo(this.origin.x, this.end.y);
			this.lineTo(this.end.x, this.end.y);
			this.lineTo(this.end.x, this.origin.y);
			this.lineTo(this.origin.x, this.origin.y);
		}
	}
}
