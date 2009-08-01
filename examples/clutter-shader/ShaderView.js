#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Clutter = imports.gi.Clutter;
GtkSource = imports.gi.GtkSource;
Gio = imports.gi.Gio;

ShaderView = new GType({
	parent: Clutter.Group.type,
	name: "ShaderView",
	init: function()
	{
		// Private

		var texture = new Clutter.Texture({filename: "bob.jpg"});
		var reflection = new Clutter.Clone({source: texture});
		var shader = new Clutter.Shader();
		
		// Public
		
		this.resize = function (width, height)
		{
			texture.x = width / 2;
			texture.y = height / 2;

			reflection.x = width / 2;
			reflection.y = (height / 2) + texture.height;
		};
				
		this.run_shader = function(button, editor)
		{
			shader.enabled = false;
			shader.fragment_source = editor.text;
			shader.compile();
			shader.enabled = true;
			
			texture.set_shader(shader);
			reflection.set_shader(shader);
		};
		
		// Implementation
		
		reflection.width = reflection.height = texture.width = texture.height = 300;
		
		reflection.anchor_x = texture.anchor_x = texture.width / 2;
		reflection.anchor_y = texture.anchor_y = texture.height / 2;
		
		reflection.rotation_angle_z = 180;
		reflection.opacity = 80;
		
		this.add_actor(texture);
		this.add_actor(reflection);
	}
});

