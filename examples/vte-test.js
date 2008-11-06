#!/usr/local/bin/seed
Seed.import_namespace("Gtk","2.0");
Seed.import_namespace("Vte");

Gtk.init(null, null);

var window = new Gtk.Window();
var scroll = new Gtk.ScrolledWindow();

var vte = new Vte.Terminal();
vte.fork_command("/bin/bash");
vte.signal.child_exited.connect(Gtk.main_quit);
vte.signal.window_title_changed.connect
    (function(terminal)
     {
	 this.set_title(terminal.get_window_title());
     }, window);


scroll.add(vte);
scroll.set_policy(Gtk.PolicyType.automatic,
		  Gtk.PolicyType.automatic);

window.add(scroll);
window.show_all();

window.width_request = 500;
window.height_request = 400;

Gtk.main();

