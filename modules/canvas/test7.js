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
		ctx.moveTo(75,25);
		ctx.quadraticCurveTo(25,25,25,62.5);
		ctx.quadraticCurveTo(25,100,50,100);
		ctx.quadraticCurveTo(50,120,30,125);
		ctx.quadraticCurveTo(60,120,65,100);
		ctx.quadraticCurveTo(125,100,125,62.5);
		ctx.quadraticCurveTo(125,25,75,25);
		ctx.stroke();
		
		return true;
});
Gtk.main();




