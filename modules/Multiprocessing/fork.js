#!/usr/bin/env seed

Seed.import_namespace("GLib");
Seed.import_namespace("Gtk");
Seed.import_namespace("Multiprocessing");

pipe = new Multiprocessing.Pipe();
child_pid = Seed.fork();

if (child_pid == 0)
{
	Gtk.init(null, null);

	childs = pipe[0];
	w = new Gtk.Window();
	l = new Gtk.Label();
	w.add(l);
	
	w.show_all();
	childs.add_watch(1, 
					 function(source, condition, label)
					 {
						 label.label = source.read();
						 return true;
					 }, l);
	Gtk.main();
}
Gtk.init(null, null);
parents = pipe[1];

w = new Gtk.Window();
l = new Gtk.Entry();
w.add(l);
w.show_all();

l.signal.activate.connect(function(entry){parents.write(entry.text);});

Gtk.main();


