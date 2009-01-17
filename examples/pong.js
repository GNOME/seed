#!/usr/bin/env seed

function angle_from_deg(x)
{
	return (((x) * 1024.0) / 360.0);
}

function circle_paint(actor)
{
	var radius = Clutter.double_to_fixed(actor.width/2);
	
	Clutter.cogl_color(green);
	Clutter.cogl_path_move_to(radius, radius);
	Clutter.cogl_path_arc(radius, radius, radius, radius,
						  angle_from_deg(0),
						  angle_from_deg(360));
	Clutter.cogl_path_close();
	Clutter.cogl_path_fill();	
}

Seed.import_namespace("Clutter");

Clutter.init(null, null);

var timeline = new Clutter.Timeline({fps:60, num_frames:30000});

var ballv_x = -2, ballv_y = 4;
var p1v_y = 0, p2v_y = 0;

timeline.signal.new_frame.connect(
	function(timeline, frame_num)
	{
		if((p_two.y + 50) > (ball.y + 15)) // UP
		{
			if(p2v_y == 0)
				p2v_y = -5;
			else
				p2v_y -= 4;
		}
		else // DOWN
		{
			if(p2v_y == 0)
				p2v_y = 5;
			else
				p2v_y += 4;
		}
	
		p_one.y += p1v_y;
		p_two.y += p2v_y;
		
		if(p1v_y > 0)
		{
			p1v_y-=5;
			if(p1v_y < 0)
				p1v_y = 0;
		}
		else if(p1v_y < 0)
		{
			p1v_y+=5;
			if(p1v_y > 0)
				p1v_y = 0;
		}
		
		if(p2v_y > 0)
		{
			p2v_y-=5;
			if(p2v_y < 0)
				p2v_y = 0;
		}
		else if(p2v_y < 0)
		{
			p2v_y+=5;
			if(p2v_y > 0)
				p2v_y = 0;
		}
		
		ball.x += ballv_x;
		ball.y += ballv_y;
		
		if((ball.y < 0) || (ball.y > (500-30)))
			ballv_y = -ballv_y;
			
		if((ball.x < 20 && ball.x > 0) && ((ball.y > p_one.y && ball.y < p_one.y+p_one.height) ||
										   (ball.y + ball.height > p_one.y &&
											ball.y + ball.height < p_one.y + p_one.height)))
		{
			ballv_x = -ballv_x;
			ballv_y += p1v_y * 2;
		}
		else if(ball.x < 10)
		{
			Seed.print("YAY YOU LOST!!");
		}
		
		if(((ball.x + ball.width) > 470 && (ball.x + ball.width) < 500) && ((ball.y > p_two.y && ball.y < p_two.y+p_two.height) ||
										   (ball.y + ball.height > p_two.y &&
											ball.y + ball.height < p_two.y + p_two.height)))
		{
			ballv_x = -ballv_x;
			ballv_y += p2v_y * 2;
		}
		else if(ball.x < 10)
		{
			Seed.print("YAY YOU LOST!!");
		}
		
		if(p_one.y < 10)
		{
			p_one.y = 10;
			p1v_y = 0;
		}
		if(p_one.y > 390)
		{
			p_one.y = 390;
			p1v_y = 0;
		}
		
		if(p_two.y < 10)
		{
			p_two.y = 10;
			p2v_y = 0;
		}
		if(p_two.y > 390)
		{
			p_two.y = 390;
			p2v_y = 0;
		}
	});

timeline.start();

var stage = new Clutter.Stage();
stage.signal.hide.connect(function(){Clutter.main_quit()});
stage.set_size(500,500);
var transp = new Clutter.Color();
var green = new Clutter.Color();
Clutter.color_parse("Green", green);
var black = new Clutter.Color();
Clutter.color_parse("Black", black);
stage.color = black;

var p_one = new Clutter.Rectangle();
p_one.color = green;
p_one.y = p_one.x = 10;
p_one.width = 20;
p_one.height = 100;

var p_two = new Clutter.Rectangle();
p_two.color = green;
p_two.y = 10;
p_two.x = 470;
p_two.width = 20;
p_two.height = 100;

stage.add_actor(p_one);
stage.add_actor(p_two);

var ball = new Clutter.Rectangle({color: transp});
ball.signal["paint"].connect(circle_paint);
ball.width = ball.height = 30;
ball.x = ball.y = 300;

stage.add_actor(ball);

stage.signal["key_press_event"].connect(
	function(stage, event)
	{
		if(event.key.keyval == 65362) // UP
		{
			if(p1v_y == 0)
				p1v_y = -5;
			else
				p1v_y -= 4;
		}
		else if(event.key.keyval == 65364) // DOWN
		{
			if(p1v_y == 0)
				p1v_y = 5;
			else
				p1v_y += 4;
		}
	
		
		return true;
	});

stage.show_all();
Clutter.main();
