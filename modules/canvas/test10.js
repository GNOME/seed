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


		for (i=0;i<6;i++){
			for (j=0;j<6;j++){
				ctx.fillStyle = 'rgb(' + Math.floor(255-42.5*i) + ',' + 
					Math.floor(255-42.5*j) + ',0)';
				ctx.fillRect(j*25,i*25,25,25);
			}
		}
		return true;
	});
Gtk.main();




