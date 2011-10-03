#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
GIRepository = imports.gi.GIRepository

gir = GIRepository.Repository.get_default()

info = gir.find_by_gtype(Gtk.Window.type)
testsuite.assert(info.get_name() == "Window")

n = GIRepository.object_info_get_n_interfaces(info)
for (i = 0; i < n; i++)
{
    property = GIRepository.object_info_get_interface(info, i)
    propertyname = property.get_name();
    testsuite.assert(propertyname == "Buildable" ||
                     propertyname == "ImplementorIface")
 }

 
testsuite.checkAsserts(3)
