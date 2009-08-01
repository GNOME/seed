#!/usr/bin/env seed

imports.gi.versions.Clutter = "1.0";

Clutter = imports.gi.Clutter;
GLib = imports.gi.GLib;
GObject = imports.gi.GObject;
Cogl = imports.gi.Cogl;

const RIPPLE_S = 2000;
const RIPPLE_W = 8;
const RIPPLE_G = 2;
const RIPPLE_N = 3;
const RIPPLE_MIND = 450;
const RIPPLE_MAXD = 2000;
const RIPPLE_WX = RIPPLE_W;

const SCREEN_W = 640;
const SCREEN_H = 480;

Clutter.init(Seed.argv);

function destroy_actor(actor){
    actor.destroy();
}

function circle_paint (actor){
    var radius = actor.width/2;

    actor.fill_color.alpha = actor.opacity;

    Cogl.set_source_color(actor.fill_color);
    Cogl.path_move_to(radius, radius);
    Cogl.path_arc(radius, radius, radius, radius, 0, 360);
    Cogl.path_line_to(radius - RIPPLE_WX/2, radius);
    Cogl.path_arc(radius, radius,
                  radius-RIPPLE_WX/2, radius-RIPPLE_WX/2,
                  0, 360);
    Cogl.path_close();
    Cogl.path_fill();
}

function ripple(stage, x, y){
    var transp = new Clutter.Color();

    var n = parseInt(Math.random()*RIPPLE_N , 10) + 1;

    for (i = 0; i < n; i++)
    {
		var actor = new Clutter.Rectangle({color: transp});

		actor.fill_color = new Clutter.Color({red: 0xff,
							  green: 0xff,
							  blue: 0xff});

		var size = ((RIPPLE_W * 2) * (i+1)) + (RIPPLE_G * i);

		actor.width = actor.height = size;
		actor.set_anchor_point_from_gravity(Clutter.Gravity.CENTER);
		actor.x = x;
		actor.y = y;
		actor.opacity = 0x80;

		stage.add_actor(actor);

		actor.signal["paint"].connect(circle_paint);

		actor.anim = actor.animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, RIPPLE_S,
		{
			width: SCREEN_W / RIPPLE_W,
			height: SCREEN_W / RIPPLE_W,
			opacity: 0
		});

		actor.anim.timeline.start();
	}
}

var stage = Clutter.Stage.get_default();

stage.width = SCREEN_W;
stage.height = SCREEN_H;
stage.color = {};

stage.signal["button_press_event"].connect(
    function(stage, event){
	ripple(stage, event.button.x, event.button.y);

	return true;
    });

stage.show();

function random_ripple(){
    ripple(stage, Math.random()*SCREEN_W, Math.random()*SCREEN_H);
    GLib.timeout_add(0, Math.random()*RIPPLE_MAXD+RIPPLE_MIND, random_ripple);

    return false;
}

random_ripple();
Clutter.main();

