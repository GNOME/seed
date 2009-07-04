#!/usr/bin/env seed
Gtk = imports.gi.Gtk;
multiprocessing = imports.multiprocessing;
os = imports.os;

var pipes = new multiprocessing.Pipe();

var child_pid = os.fork();

if (child_pid === 0){
    Gtk.init(Seed.argv);

    var id = parseInt(pipes[0].read(), 10);

    var label = new Gtk.Label({label: "Hello GtkPlug World"});
    var plug = new Gtk.Plug.c_new(id);

    plug.add(label);
    plug.show_all();
    Gtk.main();
}

Gtk.init(Seed.argv);

var window = new Gtk.Window();
window.signal.hide.connect(Gtk.main_quit);

var socket = new Gtk.Socket();
var pipe = pipes[1];

window.add(socket);
window.show_all();

pipe.write(socket.get_id());

Gtk.main();

