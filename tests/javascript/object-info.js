#!../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
GIRepository = imports.gi.GIRepository

gir = GIRepository.IRepository.get_default()

info = gir.find_by_gtype(Gtk.Window.type)
testsuite.assert(GIRepository.base_info_get_name(info) == "Window")

n = GIRepository.object_info_get_n_interfaces(info)
for (i = 0; i < n; i++)
{
    property = GIRepository.object_info_get_interface(info, i)
    propertyname = GIRepository.base_info_get_name(property)
    testsuite.assert(propertyname == "Buildable" ||
                     propertyname == "ImplementorIface")
    GIRepository.base_info_unref(property)
}

GIRepository.base_info_unref(info)

testsuite.checkAsserts(3)
