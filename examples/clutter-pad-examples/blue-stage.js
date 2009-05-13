Clutter = imports.gi.Clutter;
stage = Clutter.Stage.get_default();

var blue = new Clutter.Color();
blue.from_string("blue");

stage.color = blue;
