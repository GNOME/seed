#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Found a fooed widget
// STDERR:
Seed.import_namespace("Gtk");

Gtk.init(null, null);

function HelloLabel()
{
    this.gsuper = new Gtk.Label({label: "Hello World"});
}
HelloLabel.prototype = Seed.prototype(Gtk.Label);

function FooedHelloLabel()
{
    this.gsuper = new HelloLabel();
    this.fooed = true;
}
FooedHelloLabel.prototype = HelloLabel.prototype;

function NotALabel()
{
    this.gsuper = 3;
}

var w = new Gtk.Window();
var vbox = new Gtk.VBox();
w.add(vbox);
vbox.pack([{child: new FooedHelloLabel()}, {child: new HelloLabel()}]);

// Just make sure this doesn't crash.
try
{
	vbox.pack_start(new NotALabel());
} catch (e){}

// Verify that we get the sub-object ref out, not the actual gobject wrapper.
vbox.foreach(function(widget)
	  {
	      if (widget.fooed == true)
	      {
			widget.set_text("Fooed");
	Seed.print("Found a fooed widget");
	      }
	      else
	      {
		      widget.set_text("Not fooed");
	      }
	  });

w.show_all();
