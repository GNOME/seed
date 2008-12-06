#!/usr/local/bin/seed
Seed.import_namespace("Gdk");
Seed.import_namespace("Gtk");
Seed.import_namespace("Canvas");


Gtk.init(null, null);

w = new Gtk.Window();
d = new Gtk.DrawingArea();
w.add(d);

w.resize(150, 150);

w.show_all();

d.signal.expose_event.connect(function(){
		var cr = Gdk.cairo_create(d.window);
		var ctx = new Canvas.CairoCanvas(cr);

// Filled triangle
		ctx.beginPath();
		ctx.moveTo(25,25);
		ctx.lineTo(105,25);
		ctx.lineTo(25,105);
		ctx.fill();
		
// Stroked triangle
		ctx.beginPath();
		ctx.moveTo(125,125);
		ctx.lineTo(125,45);
		ctx.lineTo(45,125);
		ctx.closePath();
		ctx.stroke();
		
		
		return true;
});
Gtk.main();




