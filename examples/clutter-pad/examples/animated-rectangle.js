Clutter = imports.gi.Clutter;
stage = Clutter.Stage.get_default();

colors = [{red: 255, alpha: 255},
          {green: 255, alpha: 255},
          {blue: 255, alpha: 255},
          {red: 255, green: 255, alpha: 255}]

for (i = 0; i < 4; i++)
{
	rect = new Clutter.Rectangle();
	rect.color = colors[i];
	rect.x = rect.y = 0;
	rect.x += i *25;
	rect.width = rect.height = 25;
	stage.add_actor(rect);
    
	rect.anim = rect.animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, 1500+500*i,
	{
		height: 100,
		width: 100,
		x: stage.width - 100*(i+1),
		y: stage.height - 100,
		rotation_angle_z: 360
	});
}
