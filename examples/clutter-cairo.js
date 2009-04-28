#!/usr/bin/env seed

Canvas = imports.canvas;
imports.gi.versions.Clutter = "0.8"
imports.gi.versions.ClutterCairo = "0.8"

Clutter = imports.gi.Clutter;
ClutterCairo = imports.gi.ClutterCairo;

Clutter.init(null, null);

c = new ClutterCairo.ClutterCairo.c_new(100, 100);
cr = c.create();

ctx = new Canvas.CairoCanvas(cr);

ctx.fillStyle = "rgb(200,0,0)";
ctx.fillRect(10, 10, 55, 50);

ctx.fillStyle = "rgba(0, 0, 200, 0.5)";
ctx.fillRect(30, 30, 55, 50);

c.width = c.height = 100;
c.x = c.y = 100;

ctx.destroy();

s = new Clutter.Stage();
s.signal.hide.connect(Clutter.main_quit);

s.add_actor(c);

s.show_all();

Clutter.main();
