#!/usr/bin/env seed
imports.gi.versions.Clutter = "0.8";
Clutter = imports.gi.Clutter;

Clutter.init(Seed.argv);

colors = [	"blanched almond",
		"OldLace",
		"MistyRose",
		"White",
		"LavenderBlush",
		"CornflowerBlue",
		"chartreuse",
		"chocolate",
		"light coral",
		"medium violet red",
		"LemonChiffon2",
		"RosyBrown3"];

function alpha_func(alpha){
    var timeline = alpha.get_timeline();
    var frame = timeline.get_current_frame();
    var n_frames = timeline.num_frames;
    var fps = timeline.fps;
    var duration = n_frames/fps;
    var time = frame/fps;

    if ((time/=duration) < (1/2.75))
	return Clutter.ALPHA_MAX_ALPHA*(7.5625*time*time);
    else if (time < (2/2.75))
	return Clutter.ALPHA_MAX_ALPHA*(7.5625 *
					(time-=(1.5/2.75))*time+0.75);
    else if (time < (2.5/2.75))
	return Clutter.ALPHA_MAX_ALPHA*(7.5625 *
					(time-=(2.25/2.75))*time+0.9375);
    else
	return Clutter.ALPHA_MAX_ALPHA*(7.5625 *
					(time-=(2.625/2.75))*time+0.984375);
}

var stage = Clutter.Stage.get_default();
stage.signal.hide.connect(Clutter.main_quit);
var timeline = new Clutter.Timeline({fps:60, num_frames:300});
stage.show_all();

var rheight = stage.height/(colors.length);
var width = stage.width;
var rectangles = new Array(colors.length);

var black = new Clutter.Color();
Clutter.color_parse("Black", black);
var white = new Clutter.Color();
Clutter.color_parse("White", white);

stage.color = black;

for (var i = 0; i < colors.length; i++){
    var c = new Clutter.Color();
    Clutter.color_parse(colors[i],c);

    var r = new Clutter.Rectangle();
    r.width = r.height = rheight;
    r.color = c;
    r.y = i * r.height+r.height/2;

    r.anchor_x = r.height/2;
    r.anchor_y = r.height/2;

    stage.add_actor(r);
    rectangles[i] = r;
}

// Was initially intended as a test to see if performance was accetable to do
// custom animations at 60fps. Turns out to be fine, serves as an example now.

timeline.signal.new_frame.connect(
    function(timeline, frame_num){
	for (var i = 0; i < colors.length; i++){
	    rectangles[i].x += width / 600;
	    rectangles[i].rotation_angle_z += 1;
	}
    });

timeline.signal.completed.connect(
    function(timeline){

	var text = new Clutter.Label({text:"Congratulations!",
				      font_name:"Bitstream Vera Sans 40"});
	var fadeline = new Clutter.Timeline({fps:60, num_frames:200});
	var effect = new Clutter.EffectTemplate.c_new(timeline, alpha_func);

	text.show();
	stage.add_actor(text);
	text.color = white;

	text.anchor_x = text.width / 2;
	text.anchor_y = text.height / 2;

	text.x = stage.width / 2;

	text.y = -text.height;
	Clutter.effect_move(effect,
			    text,
			    text.x,
			    stage.height / 2);

	for (i in rectangles){
	    Clutter.effect_fade(effect, rectangles[i], 0);
	    Clutter.effect_move(effect, rectangles[i],
				Math.random() * stage.width,
				Math.random() * stage.height / 2 +
				stage.height / 2);
	}
    });

timeline.start();
Clutter.main();

