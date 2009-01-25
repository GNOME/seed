#!/usr/bin/env seed
Seed.import_namespace("Gtk","2.0");
Seed.import_namespace("Gst","0.10");

Gst.init(null, null);
Gtk.init(null, null);

// This is a really ugly program. Please fix it.

function oscillator(freq)
{
	this.vbox = new Gtk.VBox();
	var hbox = new Gtk.HBox();
	var vscale = new Gtk.VScale();
	var volscale = new Gtk.VScale();
	var button = new Gtk.Button({label: "Toggle"});
	
	var pipeline = new Gst.Pipeline({name: "test"});
	// No actual introspection data for audiotestsrc, so can not
	// instantiate one with a constructor, have to use element_factory,
	// likewise for the others.
	var audiosrc = Gst.ElementFactory.make("audiotestsrc", "audio");
	var audiosink = Gst.ElementFactory.make("alsasink", "sink");
	var volume = Gst.ElementFactory.make("volume", "vcontrol");
	audiosrc.freq = freq;
	
	pipeline.add(audiosrc);
	pipeline.add(audiosink);
	pipeline.add(volume);
	audiosrc.link(volume);
	volume.link(audiosink);
	
	var playing = false;
	
	vscale.adjustment.upper = 3000;
	vscale.adjustment.value = freq;
	
	volscale.adjustment.upper = 10;
	volscale.adjustment.value = volume.volume;
	
	hbox.pack_start(vscale, true, true, 10);
	hbox.pack_start(volscale, true, true, 10);
	this.vbox.pack_start(hbox, true, true, 10);
	this.vbox.pack_start(button, false, false, 10);
	
	var toggle = function(button, that) 
	{
		if (playing === false)
		{
			pipeline.set_state(Gst.State.PLAYING);
			playing = true;
		}
		else
		{
			pipeline.set_state(Gst.State.PAUSED);
			playing = false;
		}
	};
	
	var update_freq = function(range)
	{
		audiosrc.freq = range.get_value();
	};
	
	var update_vol = function(range)
	{
		volume.volume = range.get_value();
	};
	
	button.signal.clicked.connect(toggle);
	vscale.signal.value_changed.connect(update_freq);
	volscale.signal.value_changed.connect(update_vol);
}

var window = new Gtk.Window();
var button = new Gtk.Button({label: "Add Oscillator"});

window.signal.hide.connect(function () { Gtk.main_quit(); });
window.resize(600,300);
var hbox = new Gtk.HBox();

var os1 = new oscillator(523.25);
var os2 = new oscillator(659.26);
var os3 = new oscillator(783.99);

function add_oscillator(button)
{
	var os = new oscillator(300);
	hbox.pack_start(os.vbox, true, true, 10);
	os.vbox.show_all();
}
button.signal.clicked.connect(add_oscillator);

window.add(hbox);
hbox.pack_start(button, true, true, 10);
hbox.pack_start(os1.vbox, true, true, 10);
hbox.pack_start(os2.vbox, true, true, 10);
hbox.pack_start(os3.vbox, true, true, 10);
window.show_all();


Gtk.main();

