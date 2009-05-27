#!/usr/bin/env seed
Gio = imports.gi.Gio;

file = Gio.file_new_for_path(Seed.argv[1]);
input = file.read();
//get_contents is an addition in extensions/Gio.js to make life easier.
print(input.get_contents());
