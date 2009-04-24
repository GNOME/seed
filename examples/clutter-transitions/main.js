#!/usr/bin/env seed

imports.gi.versions.Clutter = "0.8";

Clutter = imports.gi.Clutter;
GLib = imports.gi.GLib;

Clutter.init(null, null);

Seed.include("transition-library.js");

var stage = new Clutter.Stage();
stage.signal.hide.connect(function(){Clutter.main_quit();});
stage.set_size(1024,768);
stage.show_all();

var black = new Clutter.Color();
Clutter.color_parse("Black", black);
stage.color = black;

var begin = new Clutter.Texture.from_file("one.jpg");
var end = new Clutter.Texture.from_file("two.jpg");

with(begin)
{
	filter_quality = Clutter.TextureQuality.HIGH;
	width = stage.width;
	height = stage.height;
}

with(end)
{
	filter_quality = Clutter.TextureQuality.HIGH;
	width = stage.width;
	height = stage.height;
}

function completed()
{
	stage.remove_all();
	animate();
}

var tr = 0;

function next_transition()
{
	transitions[tr++]({a:at, b:bt, duration:3, done:completed, direction:d_out});

	return false;
}

function animate()
{
	if(tr >= transitions.length)
		return;
		
	if(tr % 2)
	{
		at = new Clutter.CloneTexture({parent_texture:begin});
		bt = new Clutter.CloneTexture({parent_texture:end});
	}
	else
	{
		at = new Clutter.CloneTexture({parent_texture:end});
		bt = new Clutter.CloneTexture({parent_texture:begin});
	}
	
	at.width = bt.width = stage.width;
	at.height = bt.height = stage.height;

	stage.add_actor(bt);
	stage.add_actor(at);
	stage.show_all();

	//GLib.timeout_add(500, next_transition);
	trtimeline = new Clutter.Timeline();
	trtimeline.duration = 500;
	trtimeline.signal.completed.connect(next_transition);
	trtimeline.start();
}

animate();

Clutter.main();
