#!/usr/bin/env seed
Seed.import_namespace("Canvas");
Seed.import_namespace("Clutter");
Seed.import_namespace("ClutterCairo");

Clutter.init(null, null);

c = ClutterCairo.ClutterCairo._new(100, 100);
cr = c.create();

ctx = new Canvas.CairoCanvas(cr);

ctx.fillStyle = "rgb(200,0,0)";
ctx.fillRect (10, 10, 55, 50);

ctx.fillStyle = "rgba(0, 0, 200, 0.5)";
ctx.fillRect (30, 30, 55, 50);

c.width = c.height = 100;
c.x = c.y = 100;

ctx.destroy();

s = new Clutter.Stage();

s.add_actor(c);

s.show_all();

Clutter.main();
