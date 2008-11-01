#!/usr/local/bin/seed

Seed.import_namespace("Clutter");

Clutter.init(null, null);
var stage = new Clutter.Stage();
var rectangle = new Clutter.Label({text:"Never gonna give you up, never..."});
var template = new Clutter.EffectTemplate();
rectangle.width = 100;
rectangle.height = 100;
stage.add_actor(rectangle);
stage.show_all();
Clutter.main();
