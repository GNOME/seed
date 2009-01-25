#!/usr/bin/env seed

Seed.import_namespace("Clutter");
Seed.import_namespace("GL");

Clutter.init(null, null);

var s = new Clutter.Stage();
var b = new Clutter.Rectangle();

b.width = b.x = s.width/2;
b.y = 250;

s.add_actor(b);

function triangle_actor ()
{
	with(GL)
	{
		PushMatrix();
		Scalef(200,200,1);
		Begin(TRIANGLES);
			Color3f(1,0,0);
			Vertex3f(-1,-1,0);
			Color3f(0,1,0);
			Vertex3f(1,-1,0);
			Color3f(0,0,1);
			Vertex3f(0,1,0);
		End();		
		PopMatrix();
	}
}

b.signal["paint"].connect(triangle_actor);

s.show();
Clutter.main();
