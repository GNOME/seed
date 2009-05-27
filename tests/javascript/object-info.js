#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Window implements interfaces\nInterface: (Buildable|ImplementorIface)\nInterface: (Buildable|ImplementorIface)
// STDERR:

Gtk = imports.gi.Gtk;
GIRepository = imports.gi.GIRepository;

info = GIRepository.irepository_find_by_gtype(null, Gtk.Window.type);
print(GIRepository.base_info_get_name(info) + " implements interfaces");

n = GIRepository.object_info_get_n_interfaces(info);
for (i = 0; i < n; i++)
{
	property = GIRepository.object_info_get_interface(info, i);
	print("Interface: " + GIRepository.base_info_get_name(property));
	GIRepository.base_info_unref(property);
}

GIRepository.base_info_unref(info);
