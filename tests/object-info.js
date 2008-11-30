#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:Window implements interfaces\nInterface: Buildable\nInterface: ImplementorIface
// STDERR:
Seed.import_namespace("Gtk");

info = GIRepository.irepository_find_by_gtype(null, Gtk.Window.type);
Seed.print(GIRepository.base_info_get_name(info) + " implements interfaces");

n = GIRepository.object_info_get_n_interfaces(info);
for (i = 0; i < n; i++)
{
	property = GIRepository.object_info_get_interface(info, i);
	Seed.print("Interface: " + GIRepository.base_info_get_name(property));
	GIRepository.base_info_unref(property);
}

GIRepository.base_info_unref(info);
