#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Gst = imports.gi.Gst;
GObject = imports.gi.GObject;

Gst.init(Seed.argv);
Gtk.init(Seed.argv);

var pipeline = new Gst.Pipeline({name: "test"});
var unique_id = 0;

OscillatorWidget = new GType({
    parent: Gtk.VBox.type,
    name: "OscillatorWidget",
    properties: [
    	{ name: "frequency",
    	  type: GObject.TYPE_INT,
    	  default_value: 1000,
    	  minimum_value: 0,
    	  maximum_value: 3000,
    	  flags: GObject.ParamFlags.CONSTRUCT |
		         GObject.ParamFlags.READABLE |
		         GObject.ParamFlags.WRITABLE
		}
    ],
    init: function()
    {
	// Private

	var hbox = new Gtk.HBox();
	var frequency_slider = new Gtk.VScale();
	var volume_slider = new Gtk.VScale();
	var button = new Gtk.ToggleButton({label: "Enabled"});

	// No actual introspection data for audiotestsrc, so can not
	// instantiate one with a constructor, have to use element_factory,
	// likewise for the others.
	var audiosrc = Gst.ElementFactory.make("audiotestsrc", "source" + unique_id);
	var audiosink = Gst.ElementFactory.make("alsasink", "sink" + unique_id);
	var volume = Gst.ElementFactory.make("volume", "volume" + unique_id);

	unique_id++;

	var toggle_enabled = function(button, that)
	{
	    pipeline.set_state(Gst.State.PLAYING);
	    volume.volume = button.active ? volume_slider.get_value() : 0;
	};

	var set_frequency = function(range)
	{
	    this.frequency = audiosrc.freq = range.get_value();
	};

	var set_volume = function(range)
	{
	    if(button.active)
		volume.volume = range.get_value();
	};

	// Implementation

	frequency_slider.adjustment.upper = 3000;
	audiosrc.freq = frequency_slider.adjustment.value = this.frequency;

	volume_slider.adjustment.upper = 10;
	volume_slider.adjustment.value = 0.5;
	volume.volume = 0;

	pipeline.add(audiosrc);
	pipeline.add(audiosink);
	pipeline.add(volume);
	audiosrc.link_many(volume, audiosink);

	button.signal.toggled.connect(toggle_enabled);
	frequency_slider.signal.value_changed.connect(set_frequency);
	volume_slider.signal.value_changed.connect(set_volume);

	hbox.pack_start(frequency_slider, true, true, 10);
	hbox.pack_start(volume_slider, true, true, 10);
	this.pack_start(hbox, true, true, 10);
	this.pack_start(button, false, false, 10);
    }
});

var window = new Gtk.Window();
var button = new Gtk.Button({label: "Add Oscillator"});

window.signal.hide.connect(Gtk.main_quit);
window.resize(600,300);

var hbox = new Gtk.HBox();

function add_oscillator(button)
{
    var os = new OscillatorWidget();
    hbox.pack_start(os, true, true, 10);
    os.show_all();
}
button.signal.clicked.connect(add_oscillator);

window.add(hbox);
hbox.pack_start(button, true, true, 10);
hbox.pack_start(new OscillatorWidget({frequency:523.25}), true, true, 10);
hbox.pack_start(new OscillatorWidget({frequency:659.26}), true, true, 10);
hbox.pack_start(new OscillatorWidget({frequency:783.99}), true, true, 10);
window.show_all();

Gtk.main();

