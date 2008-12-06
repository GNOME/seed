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

		for (i = 0; i < 10; i++){
			ctx.lineWidth = 1+i;
			ctx.beginPath();
			ctx.moveTo(5+i*14,5);
			ctx.lineTo(5+i*14,140);
			ctx.stroke();
		}
		
		return true;
	});
Gtk.main();




