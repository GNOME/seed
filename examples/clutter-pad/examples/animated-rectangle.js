GObject = imports.gi.GObject;
Clutter = imports.gi.Clutter;
stage = Clutter.Stage.get_default();
stage.color = {alpha: 255};

rect = new Clutter.Rectangle();
rect.color = {red: 255, alpha: 255};
rect.x = rect.y = 0;
rect.width = rect.height = 25;
stage.add_actor(rect);

anim = rect.animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, 1500,
{
	height: [GObject.TYPE_INT, 100],
	width: [GObject.TYPE_INT, 100],
	x: [GObject.TYPE_INT, stage.width - 100],
	y: [GObject.TYPE_INT, stage.height - 100]
});