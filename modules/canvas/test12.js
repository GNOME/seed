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

		ctx.fillStyle = 'rgb(255,221,0)';
		ctx.fillRect(0,0,150,37.5);
		ctx.fillStyle = 'rgb(102,204,0)';
		ctx.fillRect(0,37.5,150,37.5);
		ctx.fillStyle = 'rgb(0,153,255)';
		ctx.fillRect(0,75,150,37.5);
		ctx.fillStyle = 'rgb(255,51,0)';
		ctx.fillRect(0,112.5,150,37.5);
		
		// Draw semi transparent rectangles
		for (i=0;i<10;i++){
			ctx.fillStyle = 'rgba(255,255,255,'+(i+1)/10+')';
			for (j=0;j<4;j++){
				ctx.fillRect(5+i*14,5+j*37.5,14,27.5)
					}
		}
		
		return true;
	});
Gtk.main();




