#!/usr/local/bin/seed
Seed.import_namespace("Clutter");

Clutter.init(null, null);

var stage = new Clutter.Stage();
var texture = new Clutter.Texture({filename:"bob.jpg"});
var reflection = new Clutter.CloneTexture({parent_texture:texture});
var black = Clutter.Color._new();

Clutter.color_parse("Black", black);


stage.set_color(black);
stage.add_actor(texture);
stage.add_actor(reflection);

reflection.width = reflection.height = 
   texture.height = texture.width = .55*stage.height;
texture.x = stage.width/2;
texture.y = stage.height/2;
reflection.x = texture.x
reflection.y = texture.y+texture.height;
reflection.rotation_angle_z = 180;

reflection.opacity = 80;

reflection.anchor_x = texture.anchor_x = texture.width/2;
reflection.anchor_y = texture.anchor_y = texture.height/2;

reflection.rotation_angle_y=330;
texture.rotation_angle_y=30;
timeline = new Clutter.Timeline({fps:30, num_frames: 60});
timeline.signal_new_frame.connect(function(timeline, frame_num)
			  {
			      texture.rotation_angle_y+=3;
			      reflection.rotation_angle_y-=3;
			  });
timeline.start();


stage.show_all();
Clutter.main();

