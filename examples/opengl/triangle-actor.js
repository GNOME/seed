#!/usr/bin/env seed

Clutter = imports.gi.Clutter;
GL = imports.gi.GL;

Clutter.init(null, null);

var s = Clutter.Stage.get_default();
var b = new Clutter.Rectangle();

b.width = b.x = s.width/2;
b.y = 250;

s.add_actor(b);

function triangle_actor (){
    with(GL){
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
