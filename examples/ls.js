#!/usr/local/bin/seed
Seed.import_namespace("Gio");

file = Gio.file_new_for_path(".");
enumerator = file.enumerate_children("standard::name");

while (child = enumerator.next_file())
{
	Seed.print(child.get_name());
}
