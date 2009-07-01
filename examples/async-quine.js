#!/usr/bin/env seed

GLib = imports.gi.GLib;
Gio = imports.gi.Gio;

var file = Gio.file_new_for_path("./async-quine.js");

file.read_async(0, null,
		function(source, result){
		    var stream = source.read_finish(result);
		    var dstream = new Gio.DataInputStream.c_new(stream);
		    var data = dstream.read_until("", 0);

		    print(data);
		    Seed.quit();
		});

var loop = GLib.main_loop_new();
GLib.main_loop_run(loop);
