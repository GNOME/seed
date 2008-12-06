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

		var lineJoin = ['round','bevel','miter'];
		ctx.lineWidth = 10;
		for (i=0;i<lineJoin.length;i++){
			ctx.lineJoin = lineJoin[i];
			ctx.beginPath();
			ctx.moveTo(-5,5+i*40);
			ctx.lineTo(35,45+i*40);
			ctx.lineTo(75,5+i*40);
			ctx.lineTo(115,45+i*40);
			ctx.lineTo(155,5+i*40);
			ctx.stroke();
		}

		return true;
	});
Gtk.main();




