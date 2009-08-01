#!/usr/bin/env seed

Canvas = imports.canvas;
imports.gi.versions.Clutter = "1.0";

Clutter = imports.gi.Clutter;

Clutter.init(Seed.argv);

c = new Clutter.CairoTexture();
c.set_surface_size(100, 100);
cr = c.create();

ctx = new Canvas.CairoCanvas(cr);

ctx.fillStyle = "rgb(200,0,0)";
ctx.fillRect(10, 10, 55, 50);

ctx.fillStyle = "rgba(0, 0, 200, 0.5)";
ctx.fillRect(30, 30, 55, 50);

c.width = c.height = 100;
c.x = c.y = 100;

ctx.destroy();

s = Clutter.Stage.get_default();
s.signal.hide.connect(Clutter.main_quit);

s.add_actor(c);

s.show_all();

Clutter.main();
