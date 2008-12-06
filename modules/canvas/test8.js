#!/usr/local/bin/seed
Seed.import_namespace("Gdk");
Seed.import_namespace("Gtk");
Seed.import_namespace("Canvas");


Gtk.init(null, null);

w = new Gtk.Window();
d = new Gtk.DrawingArea();
w.add(d);

w.resize(150, 200);

w.show_all();

d.signal.expose_event.connect(function(){
		var cr = Gdk.cairo_create(d.window);
		var ctx = new Canvas.CairoCanvas(cr);

		ctx.beginPath();
		ctx.moveTo(75,40);
		ctx.bezierCurveTo(75,37,70,25,50,25);
		ctx.bezierCurveTo(20,25,20,62.5,20,62.5);
		ctx.bezierCurveTo(20,80,40,102,75,120);
		ctx.bezierCurveTo(110,102,130,80,130,62.5);
		ctx.bezierCurveTo(130,62.5,130,25,100,25);
		ctx.bezierCurveTo(85,25,75,37,75,40);
		ctx.fill();
		
		return true;
});
Gtk.main();




