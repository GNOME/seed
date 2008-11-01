#!/usr/local/bin/seed
Seed.import_namespace("Gio");

if (Seed.argv.length < 3)
    file = Gio.file_new_for_path(".");
else
    file = Gio.file_new_for_path(Seed.argv[2]);

enumerator = file.enumerate_children("standard::name");

while (child = enumerator.next_file())
{
	Seed.print(child.get_name());
}
