#!/usr/bin/env seed
Seed.import_namespace("Gio");

file = Gio.file_new_for_path(Seed.argv[1]);
input = file.read();
//get_contents is an addition in extensions/Gio.js to make life easier.
Seed.print(input.get_contents());
