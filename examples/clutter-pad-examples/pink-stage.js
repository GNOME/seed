Clutter = imports.gi.Clutter;
stage = Clutter.Stage.get_default();

var pink = new Clutter.Color();
pink.from_string("pink");

stage.color = pink;
