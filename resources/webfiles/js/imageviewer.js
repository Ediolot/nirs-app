const CMAP_WIDTH_BAR_PX = 5;
const CMAP_SPACE_PX = 5;
const CMAP_FSIZE_PX = 10;
const CMAP_STEPS = 5;
const CMAP_SIZE_PX = 35;
const SCALE_MODE = PIXI.SCALE_MODES.LINEAR;

class ImageViewer {

	constructor(id, qtInterface) {
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
		this.frame    = new Frame(this.w, this.h, this.colormap, qtInterface,
															() => { this.el.css('cursor', 'crosshair'); },
															() => { this.el.css('cursor', 'default');   },
														  () => { this.draw() });


		this.pixi.stage.addChild(this.grp);
		this.pixi.renderer.autoResize = true;
		this.pixi.renderer.plugins.interaction.moveWhenInside = true;
		this.el.append(this.pixi.view);
		this.pixi.stage.addChild(this.colormap.image);
		this.frame.addTo(this.pixi.stage);
		this.colormap.addLabels(this.pixi.stage);
		this.pixi.renderer.roundPixels = true;
		this.resize();
		$(window).resize(() => { this.resize(); this.draw(); } );

		this.frame.visible = false;
		this.colormap.visible = false;
		this.draw();

		// After render update buttons width, because we need to render the text first
		this.frame.options.updateButtonsWidth();
	}

	setImage(imgWidth, imgHeight, max, min, prefOperation, data) {
		this.ratio = imgWidth / imgHeight;
		this.frame.setImage(imgWidth, imgHeight, max, min, prefOperation, data);
		this.colormap.setMaxMin(max, min);
		this.frame.visible = true;
		this.colormap.visible = true;
		this.resize();
		this.draw();
	}

	resize() {
		this.w = this.el.width();
		this.h = this.el.height();
		this.pixi.renderer.resize(this.w, this.h);
		this.frame.resize(this.w - CMAP_SIZE_PX, this.h, this.ratio);
		this.colormap.resize(this.frame.h);
	}

	draw() {
		let marginW = (this.w - this.frame.w - CMAP_SIZE_PX) / 2.0;
		let marginH = (this.h - this.frame.h) / 2.0;
		this.frame.draw(marginW, marginH);
		this.colormap.draw(marginW + this.frame.w + CMAP_SPACE_PX, marginH);
		this.pixi.render();
	}
}
