#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:Found a fooed widget
// STDERR:
Seed.import_namespace("Gtk");

Gtk.init(null, null);

function HelloLabel()
{
    this.gobject_parent = new Gtk.Label({label: "Hello World"});
}

function FooedHelloLabel()
{
    this.gobject_parent = new HelloLabel();
    this.fooed = true;
}

function NotALabel()
{
    this.gobject_parent = 3;
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
		      Seed.print("Found a fooed widget");
	      }
	  });

