#!/usr/local/bin/seed
Seed.import_namespace("Gtk");
Seed.include("json2.js");

proto = Seed.prototype(Gtk.Window);
method = Seed.introspect(proto.translate_coordinates);
Seed.print(JSON.stringify(method));