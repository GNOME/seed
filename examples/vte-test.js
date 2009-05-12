#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Vte = imports.gi.Vte;

Gtk.init(Seed.argv);

var window = new Gtk.Window();
window.signal.hide.connect(Gtk.main_quit);

var scroll = new Gtk.ScrolledWindow();

var vte = new Vte.Terminal();
vte.fork_command("/bin/bash");
vte.signal.child_exited.connect(Gtk.main_quit);
vte.signal.window_title_changed.connect(
    function(terminal){
	window.set_title(terminal.get_window_title());
    });


scroll.add(vte);
scroll.set_policy(Gtk.PolicyType.AUTOMATIC,
		  Gtk.PolicyType.AUTOMATIC);

window.add(scroll);
window.resize(600,400);
window.show_all();

Gtk.main();

