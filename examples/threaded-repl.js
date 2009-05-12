#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
GLib = imports.gi.GLib;

Gtk.init(Seed.argv);
var w = new Gtk.Window();

//Can interact with window at threaded repl.
Seed.thread_repl();

Gtk.main();
