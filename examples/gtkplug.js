#!/usr/bin/env seed
Gtk = imports.gi.Gtk;
Gio = imports.gi.Gio;
Multiprocessing = imports.multiprocessing;


var pipes = new Multiprocessing.Pipe();

var child_pid = Seed.fork();

if (child_pid === 0)
{
	Gtk.init(null, null);

	var id = parseInt(pipes[0].read(), 10);

	var label = new Gtk.Label({label: "Hello GtkPlug World"});
	var plug = new Gtk.Plug.c_new(id);

	plug.add(label);
	plug.show_all();
	Gtk.main();
}

Gtk.init(null, null);

var window = new Gtk.Window();
window.signal.hide.connect(function () { Gtk.main_quit(); });

var socket = new Gtk.Socket();
var pipe = pipes[1];

window.add(socket);
window.show_all();

pipe.write(socket.get_id());

Gtk.main();

