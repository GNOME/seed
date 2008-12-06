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
		var lineCap = ['butt','round','square'];

		ctx.strokeStyle = '#09f';
		ctx.beginPath();
		ctx.moveTo(10,10);
		ctx.lineTo(140,10);
		ctx.moveTo(10,140);
		ctx.lineTo(140,140);
		ctx.stroke();
		
		// Draw lines
		ctx.strokeStyle = "rgb(0,0,0)";
		for (i=0;i<lineCap.length;i++){
			ctx.lineWidth = 15;
			ctx.lineCap = lineCap[i];
			ctx.beginPath();
			ctx.moveTo(25+i*50,10);
			ctx.lineTo(25+i*50,140);
			ctx.stroke();
		}
		
		return true;
	});
Gtk.main();




