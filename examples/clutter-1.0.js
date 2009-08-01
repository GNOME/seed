#!/usr/bin/env seed

imports.gi.versions.Clutter = "1.0";

Clutter = imports.gi.Clutter;
GObject = imports.gi.GObject;

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

function create_rectangles(rectangles, colors){
    for (var i in colors){
	var c = new Clutter.Color();
	var r = new Clutter.Rectangle();

	c.from_string(colors[i]);

	r.width = r.height = stage.height / colors.length;
	r.color = c;
	r.set_anchor_point_from_gravity(Clutter.Gravity.CENTER);
	r.y = i * r.height + r.height/2;
	r.show();

	stage.add_actor(r);

	rectangles[i] = r;
    }
}

function animate_rectangles(rectangles){
    for (var i in rectangles){
	rectangles[i].anim =
	    rectangles[i].animate(Clutter.AnimationMode.LINEAR, 5000,
				  {
				      x: stage.width / 2,
				      rotation_angle_z: 500
				  });
	rectangles[i].anim.timeline.start();
    }
    rectangles[i].anim.timeline.signal.completed.connect(
	function(tml)
	{
	    var white = new Clutter.Color({red:255,
					   blue:255,
					   green:255,
					   alpha:255});

	    var text = new Clutter.Text({text:"Congratulations!",
					 font_name:"Bitstream Vera Sans 40", color:white});

	    text.set_anchor_point_from_gravity(Clutter.Gravity.CENTER);
	    text.x = stage.width / 2;
	    text.y = -text.height;	// Off-stage
	    stage.add_actor(text);
	    text.show();
	    text.anim =
		text.animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, 3000,
			     {
				 y: stage.height / 2
			     });
	    text.anim.timeline.start();

	    for (var i in rectangles){
		rectangles[i].anim =
		    rectangles[i].animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, 3000,
					  {
					      x: Math.random() * stage.width,
					      y: Math.random() * stage.height / 2 + stage.height / 2,
					      rotation_angle_z: rectangles[i].rotation_angle_z,
					      opacity: 0
					  });
		//rotation_angle change makes it stop spinning. don't know why it's still
		//spinning here, it really should have stopped when the timeline did.

		rectangles[i].anim.timeline.start();
	    }
	});
}

var stage = Clutter.Stage.get_default();
var rectangles = new Array(colors.length);
stage.signal.hide.connect(Clutter.main_quit);

create_rectangles(rectangles, colors);

stage.color = new Clutter.Color({alpha:255});
stage.show_all();

animate_rectangles(rectangles);

Clutter.main();

