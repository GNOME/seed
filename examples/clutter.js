#!/usr/local/bin/seed
Seed.import_namespace("Clutter");

Clutter.init(null, null);

colors = [ "blanched almond", 
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

var stage = new Clutter.Stage();
var timeline = new Clutter.Timeline({fps:60, num_frames:300});
stage.show_all();

var rheight = stage.height/(colors.length);
var width = stage.width;
var rectangles = new Array(colors.length);

var black = Clutter.Color._new();
Clutter.color_parse("Black", black);
var white = Clutter.Color._new();
Clutter.color_parse("White", white);

stage.color = black;

for (var i = 0; i < colors.length; i++)
{
	var c = Clutter.Color._new();
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

timeline.signal_new_frame.connect(
	function(timeline, frame_num)
	{
		for (var i = 0; i < colors.length; i++)
		{
			rectangles[i].x += width/600;
			rectangles[i].rotation_angle_z += 1;
		}
	});
timeline.signal_completed.connect(
	function(timeline)
	{
		
		var text = new Clutter.Label({text:"Congratulations!",
					  font_name:"Bitstream Vera Sans 40"});
		var fadeline = new Clutter.Timeline({fps:60, num_frames:200});
		var effect = Clutter.EffectTemplate._new(timeline,
							 Clutter.sine_inc_func);
		
		text.show();
		stage.add_actor(text);
		text.color = white;
		
		text.anchor_x = text.width/2;
		text.anchor_y = text.height/2;
		
		text.x = stage.width/2;
		text.y = stage.height/2;
		
		(Clutter.effect_fade(effect,text,0)).signal_completed.connect(
			Clutter.main_quit);
		for (i in rectangles)
		{
			Clutter.effect_fade(effect, rectangles[i], 0);
		}
	});

timeline.start();

Clutter.main();
	   