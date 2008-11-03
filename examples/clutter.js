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

stage = new Clutter.Stage();
timeline = new Clutter.Timeline({fps:60, num_frames:600});
stage.show_all();

rheight = stage.height/(colors.length);
width = stage.width;
rectangles = new Array(colors.length);

black = Clutter.Color._new();
Clutter.color_parse("Black", black);

stage.color = black;

for (var i = 0; i < colors.length; i++)
{
	c = Clutter.Color._new();
	Clutter.color_parse(colors[i],c);
	
	r = new Clutter.Rectangle();
	r.width = r.height = rheight;
	r.color = c;
	r.y = i * r.height+r.height/2;
	
	r.anchor_x = r.height/2;
	r.anchor_y = r.height/2;
	
	stage.add_actor(r);
	rectangles[i] = r;
}

timeline.signal_new_frame.connect(
	function(frame_num)
	{
		for (var i = 0; i < colors.length; i++)
		{
			rectangles[i].x += width/600;
			rectangles[i].rotation_angle_z += 1;
		}
	});
timeline.start();

Clutter.main();
	   