const CMAP_WIDTH_BAR_PX = 5;
const CMAP_SPACE_PX = 5;
const CMAP_FSIZE_PX = 10;
const CMAP_STEPS = 5;
const CMAP_SIZE_PX = 35;
const SCALE_MODE = PIXI.SCALE_MODES.LINEAR;
const ZOOM_ZONE_SZ_PX = 80;
const ZOOM_ZONE_FACTOR = 0.1;
const ZOOM_CURSOR_SPACE_PX = 10;

class ImageViewer {

	constructor(id) {
		PIXI.settings.SCALE_MODE = SCALE_MODE;
		PIXI.settings.PRECISION_FRAGMENT = 'highp';

		this.cursorX  = 0;
		this.cursorY  = 0;
		this.id       = id;
		this.el       = $(id);
		this.ratio    = 1;
		this.w        = this.el.width();
		this.h        = this.el.height();
		this.pixi     = new PIXI.Application({width: this.w, height: this.h, transparent: true, autoStart: false});
		this.grp      = new PIXI.Graphics();
		this.colormap = new Colormap(CMAP_WIDTH_BAR_PX, this.h, CMAP_FSIZE_PX, CMAP_STEPS, 1.0, 0.0);
		this.frame    = new Frame(this.w, this.h, this.colormap,
															(x, y, ox, oy, val) => { this.onMousemove(x, y, ox, oy, val) },
															() => { if (this.frame.options.getMode() === OptionsBar.MODES.ZOOM) this.zoom.hide = false; this.el.css('cursor', 'crosshair'); this.draw(); },
															() => { if (this.frame.options.getMode() === OptionsBar.MODES.ZOOM) this.zoom.hide = true;  this.el.css('cursor', 'default');   this.draw(); },
														  (mode) => { if (this.zoom) { this.zoom.hide = (mode === OptionsBar.MODES.DEFAULT)} }
														);

		this.zoom     = new ZoomZone(ZOOM_ZONE_SZ_PX, ZOOM_ZONE_SZ_PX, ZOOM_ZONE_FACTOR, this.frame, ZOOM_CURSOR_SPACE_PX);

		this.pixi.stage.addChild(this.grp);
		this.pixi.renderer.autoResize = true;
		this.pixi.renderer.plugins.interaction.moveWhenInside = true;
		this.el.append(this.pixi.view);
		this.pixi.stage.addChild(this.colormap.image);
		this.frame.addToStage(this.pixi.stage);
		this.colormap.addLabels(this.pixi.stage);
		this.pixi.stage.addChild(this.zoom.image);
		this.pixi.renderer.roundPixels = true;
		this.colormap.hide = true;
		this.zoom.hide     = true;
		this.frame.hide    = true;
		this.resize();
		$(window).resize(() => { this.resize() } );

		// After render update buttons width, because we need to render the text first
		this.frame.options.updateButtonsWidth();
	}

	onMousemove(x, y, ox, oy, val) {
		this.cursorX = x;
		this.cursorY = y;
		this.frame.options.valueIndicator.value = val;
		this.zoom.setCenter(ox, oy);
		this.draw();
	}

	setImage(imgWidth, imgHeight, max, min, prefOperation, data) {
		this.ratio = imgWidth / imgHeight;
		this.frame.setImage(imgWidth, imgHeight, max, min, prefOperation, data);
		this.colormap.hide = false;
		this.frame.hide    = false;
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
		this.zoom.draw(this.cursorX, this.cursorY, this.w, this.h);
		this.pixi.render();
	}
}
