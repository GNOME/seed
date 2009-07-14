#!/usr/bin/env seed

Gio = imports.gi.Gio;

// Gio.File is an interface, not an actual instantiable class, so we can not
// construct one with a "path" property. Instead it is necessary to use
// Gio.file_new_for_path, etc...

if (Seed.argv.length < 3)
    file = Gio.file_new_for_path(".");
else
    file = Gio.file_new_for_path(Seed.argv[2]);

enumerator = file.enumerate_children("standard::name,standard::size");

while ((child = enumerator.next_file())){
    Seed.printf("%s\t%d",child.get_name(), child.get_size());
}
