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


		// draw background
		ctx.fillStyle = '#FD0';
		ctx.fillRect(0,0,75,75);
		ctx.fillStyle = '#6C0';
		ctx.fillRect(75,0,75,75);
		ctx.fillStyle = '#09F';
		ctx.fillRect(0,75,75,75);
		ctx.fillStyle = '#F30';
		ctx.fillRect(75,75,75,75);
		ctx.fillStyle = '#FFF';
		
		// set transparency value
		ctx.globalAlpha = 0.2;
		
		// Draw semi transparent circles
		for (i=0;i<7;i++){
			ctx.beginPath();
			ctx.arc(75,75,10+10*i,0,Math.PI*2,true);
			ctx.fill();
		}

		return true;
	});
Gtk.main();




