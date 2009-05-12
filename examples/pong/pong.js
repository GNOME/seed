#!/usr/bin/env seed

imports.gi.versions.Clutter = "0.8";
Clutter = imports.gi.Clutter;

Clutter.init(Seed.argv);

const GLOBAL_DECEL = 0.2;

var winning_animation = 1;

Paddle = new GType({
	parent: Clutter.Group.type,
	name: "Paddle",
	init: function(klass)
	{
		// Private
		var velocity = 0;

		// Public
		this.update_position = function ()
		{
			this.y += velocity;

			// Decelerate
			if(velocity > 0)
			{
				velocity -= 4 - GLOBAL_DECEL;

				if(velocity < 0)
					velocity = 0;
			}
			else if(velocity < 0)
			{
				velocity += 4 - GLOBAL_DECEL;

				if(velocity > 0)
					velocity = 0;
			}

			// Make sure paddle is in bounds
			if(this.y < 10)
			{
				this.y = 10;
				velocity = 0;
			}
			if(this.y > (stage.height - this.height - 10))
			{
				this.y = (stage.height - this.height - 10);
				velocity = 0;
			}
		};

		this.update_velocity = function ()
		{
			if(key_up)
				this.accelerate(-1);
			else if(key_down)
				this.accelerate(1);
		};

		this.accelerate = function (direction)
		{
			if(velocity)
				velocity = velocity + (direction*4);
			else
				velocity = direction * 5;
		};

		this.set_velocity = function (new_v)
		{
			velocity = new_v;
		};

		this.get_velocity = function ()
		{
			return velocity;
		};

		this.load_texture = function ()
		{
			var bkg = new Clutter.Texture.from_file("player.png");

			bkg.filter_quality = Clutter.TextureQuality.HIGH;
			this.add_actor(bkg);
			bkg.show();
		};

		this.won_game = function ()
		{
			//var tl = new Clutter.Timeline({fps:60, num_frames:30});
			//var effect = new Clutter.EffectTemplate.c_new(tl,
			//											  Clutter.sine_inc_func);

			//Clutter.effect_scale(effect, this, 1, 1.2);
		};

		this.lost_game = function ()
		{
			//var tl = new Clutter.Timeline({fps:60, num_frames:30});
			//var effect = new Clutter.EffectTemplate.c_new(tl,
			//											  Clutter.sine_inc_func);

			//Clutter.effect_fade(effect, this, 0);
			//Clutter.effect_scale(effect, this, 20 * ((this.x > (stage.width/2)) ? -1 : 1), 0);
		};
	}
});

AIPaddle = new GType({
	parent: Paddle.type,
	name: "AIPaddle",
	init: function(klass)
	{
		// Public
		this.update_velocity = function ()
		{
			if((this.y + (this.height/2)) > (ball.y + (ball.height/2))) // UP
				this.accelerate(-1);
			else if((this.y + (this.height/2)) < (ball.y + (ball.height/2))) // DOWN
				this.accelerate(1);
		};

		this.load_texture = function ()
		{
			var bkg = new Clutter.Texture.from_file("player2.png");

			bkg.filter_quality = Clutter.TextureQuality.HIGH;
			this.add_actor(bkg);
			bkg.show();
		};
	}
});

Ball = new GType({
	parent: Clutter.Group.type,
	name: "Ball",
	init: function(klass)
	{
		// Private
		var v_x = -2;
		var v_y = 4;

		// Public
		this.set_velocity = function (vinx, viny)
		{
			v_x = vinx;
			v_y = viny;
		};

		this.update_position = function ()
		{
			this.x += v_x;
			this.y += v_y;
		};

		this.detect_collisions = function ()
		{
			// Bounce off Top/Bottom Walls
			if((this.y < 0) || (this.y > (stage.height - this.height)))
			{
				v_y = -v_y;
			}

			var bounce_paddle = function (ball, paddle)
			{
				// Should make the paddle 1-width here (and fix associated
				//  problems), in order to prevent coming in edgewise...

				var ball_left = ball.x;
				var ball_right = ball.x + ball.width;
				var ball_top = ball.y;
				var ball_bottom = ball.y + ball.height;

				var paddle_left = paddle.x;
				var paddle_right = paddle.x + paddle.width;
				var paddle_top = paddle.y;
				var paddle_bottom = paddle.y + paddle.height;

				if(	((ball_left > paddle_left && ball_left < paddle_right) ||
					 (ball_right < paddle_right && ball_right > paddle_left)) &&
					((ball_top > paddle_top && ball_top < paddle_bottom) ||
					 (ball_bottom > paddle_top && ball_bottom < paddle_bottom)))
				{
					v_x = -v_x;
					ball.x = (paddle_left > (stage.width/2)) ?
								paddle_left - ball.width : paddle_right;

					// TODO: Fix "spin" from paddle...
					//v_y += paddle.get_velocity() * 2;
				}
			};

			bounce_paddle(this, p_one);
			bounce_paddle(this, p_two);
		};

		this.check_boundaries = function ()
		{
			if(this.x < p_one.x)
			{
				p_one.lost_game();
				p_two.won_game();
			}
			else if((this.x + this.width) > (p_two.x + p_two.width))
			{
				p_one.won_game();
				p_two.lost_game();
			}
			else
			{
				return;
			}

			// Someone won, stop moving

			if(winning_animation)
				return;

			v_x = v_y = 0;

			winning_animation = 1;

			var tl = new Clutter.Timeline({fps:60, num_frames:30});
			var effect = new Clutter.EffectTemplate.c_new(tl,
														  Clutter.sine_inc_func);

			var tl2 = Clutter.effect_fade(effect, this, 0);

			tl2.signal.completed.connect(create_new_ball);
		};

		// Implementation
		var bkg = new Clutter.Texture.from_file("ball.png");

		bkg.filter_quality = Clutter.TextureQuality.HIGH;
		this.add_actor(bkg);
		bkg.show();
	}
});

var timeline = new Clutter.Timeline({fps:60, num_frames:1});

timeline.signal.new_frame.connect(
	function(timeline, frame_num)
	{
		p_one.update_position();
		p_two.update_position();

		p_one.update_velocity();
		p_two.update_velocity();

		ball.update_position();
		ball.detect_collisions();
		ball.check_boundaries();

		timeline.rewind();
	});

timeline.start();

var black = new Clutter.Color();
Clutter.color_parse("Black", black);

var stage = new Clutter.Stage();
stage.signal.hide.connect(Clutter.main_quit);
stage.set_size(500,500);
stage.color = black;

var p_one = new Paddle();
var p_two = new AIPaddle();

p_one.load_texture();
p_two.load_texture();

p_one.y = p_one.x = 10;

p_two.y = stage.height - p_two.height - 10;
p_two.x = stage.width - p_two.width - 10;

stage.add_actor(p_one);
stage.add_actor(p_two);

var midline = new Clutter.Texture.from_file("midline.png");
midline.filter_quality = Clutter.TextureQuality.HIGH;
midline.x = (stage.width/2) - (midline.width/2);
midline.opacity = 30;
stage.add_actor(midline);

var ball = null;

function create_new_ball ()
{
	if(!ball)
	{
		ball = new Ball();
		stage.add_actor(ball);
	}

	var tl = new Clutter.Timeline({fps:60, num_frames:30});
	var effect = new Clutter.EffectTemplate.c_new(tl,
												  Clutter.sine_inc_func);

	Clutter.effect_fade(effect, ball, 255);

	ball.width = ball.height = 30;
	ball.x = ball.y = 300;

	ball.set_velocity(Math.random() * 12 - 6, Math.random() * 12 - 6);

	winning_animation = 0;
}

create_new_ball();

key_up = key_down = 0;

stage.signal["key_press_event"].connect(
	function(stage, event)
	{
		if(event.key.keyval == 65362) // UP
			key_up = 1;
		else if(event.key.keyval == 65364) // DOWN
			key_down = 1;

		return true;
	});

stage.signal["key_release_event"].connect(
	function(stage, event)
	{
		if(event.key.keyval == 65362) // UP
			key_up = 0;
		else if(event.key.keyval == 65364) // DOWN
			key_down = 0;

		return true;
	});

stage.show_all();
Clutter.main();
