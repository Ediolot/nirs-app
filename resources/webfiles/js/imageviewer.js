const CMAP_WIDTH_BAR_PX = 5;
const CMAP_SPACE_PX = 5;
const CMAP_FSIZE_PX = 10;
const CMAP_STEPS = 4;
const CMAP_SIZE_PX = 45;

class ImageViewer {

	constructor(id) {
		this.id       = id;
		this.el       = $(id);
		this.ratio    = 1;
		this.w        = this.el.width();
		this.h        = this.el.height();
		this.pixi     = new PIXI.Application({width: this.w, height: this.h, transparent: true, autoStart: false});
		this.grp      = new PIXI.Graphics();
		this.colormap = new Colormap(CMAP_WIDTH_BAR_PX, this.h, CMAP_FSIZE_PX, CMAP_STEPS, 1.0, 0.0);
		this.frame    = new Frame(this.w, this.h, this.colormap);
		this.pixi.stage.addChild(this.grp);
		this.pixi.renderer.autoResize = true;
		this.el.append(this.pixi.view);
		this.pixi.stage.addChild(this.colormap.image);
		this.pixi.stage.addChild(this.frame.image);
		this.colormap.addLabels(this.pixi.stage);
		this.colormap.hide = true;
		this.resize();
		$(window).resize(() => { this.resize() } );
	}

	setImage(imgWidth, imgHeight, max, min, prefOperation, data) {
		this.ratio = imgWidth / imgHeight;
		this.frame.setImage(imgWidth, imgHeight, max, min, prefOperation, data);
		this.colormap.hide = false;
		this.colormap.setMaxMin(max, min);
		this.resize();
	}

	resize() {
		this.w = this.el.width();
		this.h = this.el.height();
		this.pixi.renderer.resize(this.w, this.h);
		this.frame.resize(this.w - CMAP_SIZE_PX, this.h, this.ratio);
		this.colormap.resize(this.frame.h);
		this.draw();
	}

	draw() {
		let marginW = (this.w - this.frame.w - CMAP_SIZE_PX) / 2.0;
		let marginH = (this.h - this.frame.h) / 2.0;
		this.frame.draw(marginW, marginH);
		this.colormap.draw(marginW + this.frame.w + CMAP_SPACE_PX, marginH);
		this.pixi.render();
	}
}
