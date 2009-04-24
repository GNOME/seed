#!/usr/bin/env seed


global_fps = 60;
var d_left = 0;
var d_up = 1;
var d_down = 2;
var d_right = 3;
var d_in = -1;
var d_out = 1;

function bounce_alpha(alpha)
{
	try
	{
		timeline = alpha.get_timeline();
		frame = timeline.get_current_frame();
		n_frames = timeline.num_frames;
		fps = timeline.fps;
		duration = n_frames/fps;
		time = frame/fps;
		
		if ((time/=duration) < (1/2.75))
		return Clutter.ALPHA_MAX_ALPHA*(7.5625*time*time);
		else if (time < (2/2.75))
		return Clutter.ALPHA_MAX_ALPHA*(7.5625 * 
						(time-=(1.5/2.75))*time+0.75);
		else if (time < (2.5/2.75))
		return Clutter.ALPHA_MAX_ALPHA*(7.5625 *
						(time-=(2.25/2.75))*time+0.9375);
		else
		return Clutter.ALPHA_MAX_ALPHA*(7.5625 * (time-=
							  (2.625/2.75))*time+0.984375);
		
	}
	catch (e)
	{
		Seed.print(e.message);
	}
}

function fade(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.ramp_inc_func);

		b.opacity = 0;

		Clutter.effect_fade(effect, b, 255);
		var cline = Clutter.effect_fade(effect, a, 0);

		cline.signal.completed.connect(done);

		timeline.start();
	}
}

function fly(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.sine_inc_func);

		b.depth = (-direction) * 250;
		b.opacity = 0;
	
		Clutter.effect_depth(effect, b, 0);
		Clutter.effect_depth(effect, a, direction*250);
		Clutter.effect_fade(effect, b, 255);
		var cline = Clutter.effect_fade(effect, a, 0);
	
		cline.signal.completed.connect(done);

		timeline.start();
	}
}

function push(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.smoothstep_inc_func);

		var sign = (direction % 2) ? -1 : 1;

		if(direction == d_up || direction == d_down)
		{
			b.y = stage.height * (-sign);
			Clutter.effect_move(effect, a, 0, stage.height * sign);
		}
		else
		{
			b.x = stage.width * sign;
			Clutter.effect_move(effect, a, stage.width * (-sign), 0);
		}

		var cline = Clutter.effect_move(effect, b, 0, 0);

		cline.signal.completed.connect(done);

		timeline.start();
	}
}

function reveal(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.smoothstep_inc_func);

		var sign = (direction % 2) ? -1 : 1;
		var cline;

		if(direction == d_up || direction == d_down)
			cline = Clutter.effect_move(effect, a, 0, stage.height * sign);
		else
			cline = Clutter.effect_move(effect, a, stage.width * (-sign), 0);

		cline.signal.completed.connect(done);

		timeline.start();
	}
}

function move_in(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.smoothstep_inc_func);

		var sign = (direction % 2) ? -1 : 1;
		b.raise_top();

		if(direction == d_up || direction == d_down)
			b.y = stage.height * (-sign);
		else
			b.x = stage.width * sign;

		var cline = Clutter.effect_move(effect, b, 0, 0);

		cline.signal.completed.connect(done);

		timeline.start();
	}
}

function drop(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, bounce_alpha);

		b.y = - stage.height;
		b.raise_top();

		var cline = Clutter.effect_move(effect, b, 0, 0);

		cline.signal.completed.connect(done);

		timeline.start();
	}
}

function pivot(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.sine_inc_func);

		b.raise_top();
		b.rotation_angle_z = 270;

		var cline = Clutter.effect_rotate(effect, b, 2, 0, 0, 0, 0, 0);

		cline.signal.completed.connect(done);

		timeline.start();
	}
}

function fall_out(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.sine_inc_func);

		var cline = Clutter.effect_rotate(effect, a, 0, 270, 0, a.height, 0, 1);

		cline.signal.completed.connect(done);

		timeline.start();
	}
}

function scale(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.smoothstep_inc_func);

		a.anchor_x = a.width/2;
		a.anchor_y = a.height/2;

		a.x = stage.width/2;
		a.y = stage.height/2;

		var s = (direction == d_in) ? 0 : 10;

		Clutter.effect_scale(effect, a, s, s);
		var cline = Clutter.effect_fade(effect, a, 0);

		cline.signal.completed.connect(done);

		timeline.start();
	}
}

function tiles_fly(t)
{
	with(t)
	{
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps, fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.smoothstep_inc_func);

		var tiles = [ ];
		var tile_w = stage.width / 8;
		var tile_h = stage.height / 6;

		for(var j = 0; j < stage.height; j += tile_h)
		{
			for(var i = 0; i < stage.width; i += tile_w)
			{
				var g = new Clutter.CloneTexture({parent_texture:a.parent_texture});
				g.set_clip(i,j,tile_w,tile_h);

				g.width = a.width;
				g.height = a.height;

				stage.add_actor(g);
				tiles.push(g);
			}
		}
		
		stage.show_all();
		
		a.opacity = 0;

		for(var til in tiles)
		{
			Clutter.effect_depth(effect, tiles[til], Math.random() * 5000);
			var cline = Clutter.effect_fade(effect, tiles[til], 0);
		}

		cline.signal.completed.connect(function (timeline, tiles)
									   {
										   for(var til in tiles)
										   {
											   tiles[til].destroy();
										   }
										   
										   done();
									   }, tiles);

		timeline.start();
	}
}

function tiles(t)
{
	var timeline;
	with(t)
	{
		var tiles = [ ];
		var tile_w = stage.width / 8;
		var tile_h = stage.height / 6;

		for(var j = 0; j < stage.height; j += tile_h)
		{
			for(var i = 0; i < stage.width; i += tile_w)
			{
				var g = new Clutter.CloneTexture({parent_texture:a.parent_texture});
				g.set_clip(i,j,tile_w,tile_h);

				g.width = a.width;
				g.height = a.height;

				g.anchor_x = i + tile_w/2;
				g.anchor_y = j + tile_h/2;

				g.x = i + tile_w/2;
				g.y = j + tile_h/2;

				stage.add_actor(g);
				tiles.push(g);
			}
		}
		
		stage.show_all();
		
		a.opacity = 0;

		for(var til in tiles)
		{
			timeline = new Clutter.Timeline({num_frames:(Math.random()+0.01)*duration*global_fps,
											 fps:global_fps});
			var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.ramp_inc_func);

			Clutter.effect_rotate(effect, tiles[til], 1, 180, 0, 0, 0, Math.random()>0.5 ? 1 : 0);
			Clutter.effect_fade(effect, tiles[til], 0);
		}

		timeline = new Clutter.Timeline({num_frames:duration*global_fps,
										 fps:global_fps});

		timeline.signal.completed.connect(function (timeline, tiles)
										  {
											  for(var til in tiles)
											  {
												  tiles[til].destroy();
											  }
											  
											  done();
										  }, tiles);

		timeline.start();
	}
}

function tiles_across(t)
{
	var timeline;
	with(t)
	{
		var tiles = [ ];
		var tile_w = stage.width / 8;
		var tile_h = stage.height / 6;

		for(var j = 0; j < stage.height; j += tile_h)
		{
			for(var i = 0; i < stage.width; i += tile_w)
			{
				var g = new Clutter.CloneTexture({parent_texture:a.parent_texture});
				g.set_clip(i,j,tile_w,tile_h);

				g.width = a.width;
				g.height = a.height;

				g.anchor_x = i + tile_w/2;
				g.anchor_y = j + tile_h/2;

				g.x = i + tile_w/2;
				g.y = j + tile_h/2;

				stage.add_actor(g);
				tiles.push(g);
			}
		}
		
		stage.show_all();
		
		a.opacity = 0;

		for(var til in tiles)
		{
			var multiplier = (((tiles[til].anchor_x*tiles[til].anchor_y)/
								 (stage.width*stage.height)) + 0.01);
			
			timeline = new Clutter.Timeline({num_frames:multiplier*duration*global_fps,
											 fps:global_fps});
			var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.ramp_inc_func);

			Clutter.effect_rotate(effect, tiles[til], 1, 180, 0, 0, 0);
			Clutter.effect_fade(effect, tiles[til], 0);
		}

		timeline = new Clutter.Timeline({num_frames:duration*global_fps,
										 fps:global_fps});
		
		timeline.signal.completed.connect(function (timeline, tiles)
									   {
										   for(var til in tiles)
										   {
											   tiles[til].destroy();
										   }
										   
										   done();
									   }, tiles);

		timeline.start();
	}
}

function doorway(t)
{
	with(t)
	{
		var reflection = new Clutter.CloneTexture({parent_texture:b.parent_texture});
		var left_tile = new Clutter.CloneTexture({parent_texture:a.parent_texture});
		var right_tile = new Clutter.CloneTexture({parent_texture:a.parent_texture});

		reflection.width = b.width;
		reflection.height = b.height;
		
		reflection.anchor_x = b.anchor_x = b.width/2;
		reflection.anchor_y = b.anchor_y = b.height/2;

		reflection.x = b.x = stage.width/2;
		b.y = stage.height/2;
		reflection.y = b.y + b.height;

		reflection.rotation_angle_z = 180;
		reflection.rotation_angle_y = 180;

		reflection.opacity = 80;

		stage.remove_actor(b);

		var group = new Clutter.Group();
		group.add_actor(reflection);
		group.add_actor(b);

		group.anchor_x = b.width/2;
		group.anchor_y = b.height/2;

		group.x = stage.width/2;
		group.y = stage.height/2;

		stage.add_actor(group);

		left_tile.width = right_tile.width = a.width;
		left_tile.height = right_tile.height = a.height;

		left_tile.set_clip(0,0,left_tile.width/2,left_tile.height);
		right_tile.set_clip(right_tile.width/2,0,right_tile.width/2,right_tile.height);

		a.opacity = 0;

		group.scale_x = group.scale_y = 0.5;

		stage.add_actor(left_tile);
		stage.add_actor(right_tile);

		stage.show_all();
		
		var timeline = new Clutter.Timeline({num_frames:duration*global_fps,
											 fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.smoothstep_inc_func);

		Clutter.effect_rotate(effect, left_tile, 1, 10, 0, 0, 0, 0);
		Clutter.effect_rotate(effect, right_tile, 1, 360-10, right_tile.width, 0, 0, 1);

		Clutter.effect_move(effect, left_tile, 0-(left_tile.width/2), 0);
		Clutter.effect_move(effect, right_tile, stage.width - (right_tile.width/2), 0);

		Clutter.effect_fade(effect, left_tile, 0);
		Clutter.effect_fade(effect, right_tile, 0);

		Clutter.effect_scale(effect, group, 1, 1);

		timeline.signal.completed.connect(function ()
									   {
										   group.remove_actor(b);
										   stage.add_actor(b);
										   stage.show_all();

										   left_tile.destroy();
										   right_tile.destroy();
										   group.destroy();
										   reflection.destroy();
										   done();
									   });

		timeline.start();
	}
}

function color_planes(t)
{
	with(t)
	{		
		a.opacity = b.opacity = 0;
		var r_part = new Clutter.CloneTexture({parent_texture:a.parent_texture});
		var g_part = new Clutter.CloneTexture({parent_texture:a.parent_texture});
		var b_part = new Clutter.CloneTexture({parent_texture:a.parent_texture});

		r_part.width = g_part.width = b_part.width = a.width;
		r_part.height = g_part.height = b_part.height = a.height;

	   r_part.depth = g_part.depth = b_part.depth = a.depth;
		
		var r_shader = new Clutter.Shader();
		r_shader.fragment_source = "						   \
uniform sampler2D tex;										   \
void main ()												   \
{															   \
	vec4 color = texture2D (tex, vec2(gl_TexCoord[0]));		   \
	color.rgb = vec3(1.0, 0.0, 0.0) * color.rgb;			   \
	color.a = color.r;										   \
	gl_FragColor = color;									   \
}";
		var g_shader = new Clutter.Shader();
		g_shader.fragment_source = "						   \
uniform sampler2D tex;										   \
void main ()												   \
{															   \
	vec4 color = texture2D (tex, vec2(gl_TexCoord[0]));		   \
	color.rgb = vec3(0.0, 1.0, 0.0) * color.rgb;			   \
	color.a = color.g;										   \
	gl_FragColor = color;									   \
}";
		var b_shader = new Clutter.Shader();
		b_shader.fragment_source = "						   \
uniform sampler2D tex;										   \
void main ()												   \
{															   \
	vec4 color = texture2D (tex, vec2(gl_TexCoord[0]));		   \
	color.rgb = vec3(0.0, 0.0, 1.0) * color.rgb;			   \
	color.a = color.b;										   \
	gl_FragColor = color;									   \
}";
		r_shader.compile();
		g_shader.compile();
		b_shader.compile();

		r_part.set_shader(r_shader);
		g_part.set_shader(g_shader);
		b_part.set_shader(b_shader);

		stage.add_actor(r_part);
		stage.add_actor(g_part);
		stage.add_actor(b_part);

		stage.show_all();

		r_part.opacity = 254;
		g_part.opacity = 254;
		b_part.opacity = 254;

		r_part.anchor_x = g_part.anchor_x = b_part.anchor_x = r_part.width/2;
		r_part.anchor_y = g_part.anchor_y = b_part.anchor_y = r_part.height/2;

		r_part.x = g_part.x = b_part.x = stage.width/2;
		r_part.y = g_part.y = b_part.y = stage.height/2;

		var timeline = new Clutter.Timeline({num_frames:0.2*duration*global_fps,
											 fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.sine_inc_func);

		Clutter.effect_depth(effect, r_part, 200);
		Clutter.effect_depth(effect, g_part, 0);
		Clutter.effect_depth(effect, b_part, -200);

		timeline.start();

		timeline.signal.completed.connect(function(){

		var timeline = new Clutter.Timeline({num_frames:0.6*duration*global_fps,
											 fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.smoothstep_inc_func);

		Clutter.effect_rotate(effect, r_part, 1, 180, 0, 0, -200, 0);
		Clutter.effect_rotate(effect, g_part, 1, 180, 0, 0, 0, 0);
		Clutter.effect_rotate(effect, b_part, 1, 180, 0, 0, 200, 0);

		timeline.start();

		timeline.signal.completed.connect(function(){

		var timeline = new Clutter.Timeline({num_frames:0.2*duration*global_fps,
											 fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.sine_inc_func);

		Clutter.effect_depth(effect, r_part, 400);
		Clutter.effect_depth(effect, g_part, 0);
		Clutter.effect_depth(effect, b_part, -400);

		timeline.start();
});
});
	}
}

function blur(t)
{
	with(t)
	{		
		a.opacity = b.opacity = 0;
		var blur = new Clutter.CloneTexture({parent_texture:a.parent_texture});
		blur.width = a.width;
		blur.height = a.height;
		
		var blur_shader = new Clutter.Shader();
		blur_shader.fragment_source = "						   \
uniform sampler2D tex;										   \
uniform float radius = 1;									   \
uniform float x_step = .00097, y_step = 0.0013;				   \
void main ()												   \
{															   \
	 vec4 color = texture2D (tex, vec2(gl_TexCoord[0]));	   \
	 float u, v;											   \
	 int count = 1;											   \
	 for (u=-radius;u<radius;u++)							   \
	   for (v=-radius;v<radius;v++)							   \
		 {													   \
		   color += texture2D(tex,							   \
				vec2(gl_TexCoord[0].s + u * 1.0 * x_step,	   \
					 gl_TexCoord[0].t + v * 1.0 * y_step));	   \
		   count ++;										   \
		 }													   \
	 color = color / float(count);							   \
	 gl_FragColor = color;									   \
	 gl_FragColor = gl_FragColor * gl_Color;				   \
}";
		blur_shader.compile();

		blur.set_shader(blur_shader);

		stage.add_actor(blur);

		stage.show_all();

		blur.opacity = 254;

		var timeline = new Clutter.Timeline({num_frames:duration*global_fps,
											 fps:global_fps});
		var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.ramp_inc_func);

		timeline.signal.new_frame.connect(function (tl, frame)
			   {
					   var num = frame / (duration * global_fps);
					   blur.set_shader_param("radius", num * 10);
			   });

		timeline.signal.completed.connect(function ()
		{
			var timeline = new Clutter.Timeline({num_frames:duration*global_fps,
												 fps:global_fps});
			var effect = new Clutter.EffectTemplate.c_new(timeline, Clutter.smoothstep_inc_func);
			b.opacity = 0;
			b.raise_top();
			Clutter.effect_fade(effect, b, 255);

			timeline.start();
		});

		timeline.start();
	}
}

transitions = [ fade, fly, push, reveal, move_in, drop, pivot, fall_out, scale, tiles_fly, tiles, tiles_across, doorway, color_planes, blur ];

