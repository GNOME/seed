#!/usr/local/bin/seed
Seed.import_namespace("Gdk");
Seed.import_namespace("Gtk");
Seed.import_namespace("Canvas");


Gtk.init(null, null);

w = new Gtk.Window();
d = new Gtk.DrawingArea();
w.add(d);

w.resize(300, 150);

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
		ctx.translate(150, 0);
		for (i=0;i<6;i++){
			for (j=0;j<6;j++){
				ctx.strokeStyle = 'rgb(0,' + Math.floor(255-42.5*i) + ',' + 
					Math.floor(255-42.5*j) + ')';
				ctx.beginPath();
				ctx.arc(12.5+j*25,12.5+i*25,10,0,Math.PI*2,true);
				ctx.stroke();
			}
		}


		return true;
	});
Gtk.main();




