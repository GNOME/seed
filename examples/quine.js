#!/usr/local/bin/seed
Seed.import_namespace("Gio");

file = Gio.file_new_for_path(Seed.argv[1])
input = file.read();
stream = Gio.DataInputStream._new(input);

line = stream.read_until("", 0);
Seed.print(line);