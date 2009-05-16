cairo = imports.cairo;
Clutter = imports.gi.Clutter;

var stage = Clutter.Stage.get_default();

var c1 = Math.random();
var c2 = Math.random();

function fluffy(c) {
	var texture = new Clutter.CairoTexture();
	texture.set_surface_size(159, 163);
	var context = texture.create();
	var cr = new cairo.Context.steal(context);
	cr.set_source_rgba(c.r, c.g, c.b, c.a);
	cr.new_path ();
	cr.move_to (59, 20);
	for (var i = 0; i < 10; ++i) {
		cr.rel_curve_to (10*c1, -25*c1, 30*c2, -25*c2, 40, 0);
		cr.rotate(3.14*2/10);
	}
	cr.close_path ();
	cr.fill ();
	cr.destroy();
	return texture;
}

var f = fluffy({r:1, g:0, b:1, a:1});
stage.add_actor(f);
f.set_anchor_point_from_gravity(Clutter.Gravity.CENTER);
f.x = 100;
f.y = 100;
f.width = 150;
f.height = 150;
f.anim = f.animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, 2000, {rotation_angle_z: 180, width: 200, height: 200});

f2 = fluffy({r:0, g:1, b:1, a:1});
stage.add_actor(f2);
f2.set_anchor_point_from_gravity(Clutter.Gravity.CENTER);
f2.x = 100;
f2.y = 100;
f2.width = 100;
f2.height = 100;
f2.anim = f2.animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, 2000, {rotation_angle_z: -180, width: 150, height: 150});

f2 = fluffy({r:1, g:1, b:0, a:1});
stage.add_actor(f2);
f2.set_anchor_point_from_gravity(Clutter.Gravity.CENTER);
f2.x = 100;
f2.y = 100;
f2.width = 50;
f2.height = 50;
f2.anim = f2.animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, 2000, {rotation_angle_z: 180, width: 100, height: 100});
