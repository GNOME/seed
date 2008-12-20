#!/usr/bin/env seed
Seed.import_namespace("Gtk");
Seed.import_namespace("GLib");
Gtk.init(null, null);
var w = new Gtk.Window();

//Can interact with window at threaded repl.
Seed.thread_repl();

Gtk.main();