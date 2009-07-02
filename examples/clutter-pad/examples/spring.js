Clutter = imports.gi.Clutter;
stage = Clutter.Stage.get_default();

var vs = 0, as = 0, f = 0, dragging = false;

var spring = new Clutter.Rectangle();
spring.height = 200;
spring.width = spring.height / 2;
spring.anchor_x = spring.width / 2;
spring.x = stage.width / 2;
spring.color = {red: 100, green: 100, blue: 100, alpha: 255};

var handle = new Clutter.Rectangle();
handle.width = stage.width / 2;
handle.height = 50;
handle.anchor_x = handle.width / 2;
handle.x = stage.width / 2;
handle.y = spring.height;

function click()
{
	dragging = true;
	return false;
}

function release()
{
	dragging = false;
	return false;
}

function drag(actor, event)
{
	if(!dragging)
		return false;

	spring.height = event.motion.y;

	if(spring.height > 300)
		spring.height = 300;
	else if(spring.height < 100)
		spring.height = 100;

	update_relative_sizes();
	return false;
}

function update_relative_sizes()
{
	handle.y = spring.height;

	spring.width = spring.height / 2;
	spring.anchor_x = spring.width / 2;

	var red = spring.height > 200 ? 100 + (spring.height - 200) : 100;
	var green = spring.height < 200 ? 100 + (200 - spring.height) : 100;

	spring.color = {red: red, green: green, blue: 100, alpha: 255};
}

function update_spring()
{
	if(dragging)
		return false;

	f = -0.1 * (spring.height - 200);
	as = f / 0.8;
	vs = 0.92 * (vs + as);
	spring.height += vs;

	if(Math.abs(vs) < 0.1)
		vs = 0.0;

	update_relative_sizes();

	return false;
}

stage.add_actor(handle);
stage.add_actor(spring);

stage.signal.button_press_event.connect(click);
stage.signal.button_release_event.connect(release);
stage.signal.motion_event.connect(drag);

var update = new Clutter.Timeline({duration: 500});
update.loop = true;
update.signal.new_frame.connect(update_spring);
update.start();
