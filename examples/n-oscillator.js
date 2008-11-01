#!/usr/local/bin/seed
Seed.import_namespace("Gtk","2.0");
Seed.import_namespace("Gst","0.10");

Gtk.init(null, null);

function oscillator(freq)
{
	this.vbox = new Gtk.VBox();
	this.hbox = new Gtk.HBox();
	this.vscale = new Gtk.VScale();
	this.volscale = new Gtk.VScale();
	this.button = new Gtk.Button({label: "Toggle"});
	
	this.pipeline = new Gst.Pipeline({name: "test"});
	this.audiosrc = Gst.element_factory_make("audiotestsrc", "audio");
	this.audiosink = Gst.element_factory_make("alsasink", "sink");
	this.volume = Gst.element_factory_make("volume", "vcontrol");
	this.audiosrc.freq = freq;
	
	this.pipeline.add(this.audiosrc);
	this.pipeline.add(this.audiosink);
	this.pipeline.add(this.volume);
	this.audiosrc.link(this.volume);
	this.volume.link(this.audiosink);
	
	this.playing = false;
	
	var adjustment = this.vscale.get_adjustment();
	adjustment.upper = 3000;
	adjustment.value = freq;
	
	var adjustment = this.volscale.get_adjustment();
	adjustment.upper = 10;
	adjustment.value = this.volume.volume;
	
	this.hbox.pack_start(this.vscale, true, true, 10);
	this.hbox.pack_start(this.volscale, true, true, 10);
	this.vbox.pack_start(this.hbox, true, true, 10);
	this.vbox.pack_start(this.button, false, false, 10);
	
	this.toggle = function(button) 
	{
		if (this.playing == false)
		{
			this.pipeline.set_state(Gst.State.playing);
			this.playing = true;
		}
		else
		{
			this.pipeline.set_state(Gst.State.paused);
			this.playing = false;
		}
	}
	this.update_freq = function(range)
	{
		this.audiosrc.freq = range.get_value();
	}
	this.update_vol = function(range)
	{
		this.volume.volume = range.get_value();
	}
	this.button.signal_clicked.connect(this.toggle, this);
	this.vscale.signal_value_changed.connect(this.update_freq, this);
	this.volscale.signal_value_changed.connect(this.update_vol, this);
}

function end_program()
{
	Gtk.main_quit();
}

var window = new Gtk.Window();
var button = new Gtk.Button({label: "Add Oscillator"});

window.signal_hide.connect(end_program);
window.resize(600,300);
var hbox = new Gtk.HBox();

var os1 = new oscillator(500);
var os2 = new oscillator(700);

function add_oscillator(button)
{
	var os = new oscillator(300);
	this.pack_start(os.vbox, true, true, 10);
	os.vbox.show_all();
}
button.signal_clicked.connect(add_oscillator, hbox);

window.add(hbox);
hbox.pack_start(button, true, true, 10);
hbox.pack_start(os1.vbox, true, true, 10);
hbox.pack_start(os2.vbox, true, true, 10);
window.show_all();


Gtk.main();

