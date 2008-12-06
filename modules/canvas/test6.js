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
		for (i=0;i<4;i++){
			for(j=0;j<3;j++){
				ctx.beginPath();
				var x              = 25+j*50;               // x coordinate
				var y              = 25+i*50;               // y coordinate
				var radius         = 20;                    // Arc radius
				var startAngle     = 0;                     // Starting point on circle
				var endAngle       = Math.PI+(Math.PI*j)/2; // End point on circle
				var anticlockwise  = i%2==0 ? false : true; // clockwise or anticlockwise
				
				ctx.arc(x,y,radius,startAngle,endAngle, anticlockwise);
				
				if (i>1){
					ctx.fill();
				} else {
					ctx.stroke();
				}
			}
		}
		
		
		return true;
});
Gtk.main();




