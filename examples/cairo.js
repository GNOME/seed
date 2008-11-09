#!/usr/local/bin/seed
Seed.import_namespace("Cairo");
Seed.import_namespace("Gdk");
Seed.import_namespace("Gtk");

Gtk.init(null, null);

w = new Gtk.Window();
w.signal.hide.connect(Gtk.main_quit);
w.app_paintable = true;

w.signal.expose_event.connect(
      function(widget, event)
      {
	  
	  cairo = Gdk.cairo_create(widget.window);
	  pattern = Cairo.pattern_create_linear(0, 0, 0, 256);
	  
	  Cairo.pattern_add_color_stop_rgba(pattern, 1, 0, 0, 0, 1);
	  Cairo.pattern_add_color_stop_rgba(pattern, 0, 1, 1, 1, 1);
	  Cairo.rectangle(cairo, 0, 0, 256, 256);
	  Cairo.set_source(cairo, pattern);
	  Cairo.fill(cairo);
	  
	  Cairo.pattern_destroy(pattern);
	  
	  pattern = Cairo.pattern_create_radial(115.2, 102.4, 25.6,
						 102.4, 102.4, 128.0);
	  Cairo.pattern_add_color_stop_rgba(pattern, 0, 1, 1, 1, 1);
	  Cairo.pattern_add_color_stop_rgba(pattern, 1, 0, 0, 0, 1);
	  Cairo.set_source(cairo, pattern);
	  Cairo.arc(cairo, 128, 128, 76.8, 0, 2*Math.PI);
	  Cairo.fill(cairo);
	  Cairo.pattern_destroy(pattern);
	  
      });

w.show_all();
w.resize(256, 256);
Gtk.main();