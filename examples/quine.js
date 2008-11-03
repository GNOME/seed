#!/usr/local/bin/seed
Seed.import_namespace("Gio");

file = Gio.file_new_for_path(Seed.argv[1])
input = file.read();

Seed.print(input.get_contents());