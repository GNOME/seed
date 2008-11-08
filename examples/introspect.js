#!/usr/bin/env seed
Seed.import_namespace("Gtk");

proto = Seed.prototype(Gtk.Window);
method = Seed.introspect(proto.translate_coordinates);
//JSON.stringify and JSON.fromString come from json2.js available on json.org
//Also in extensions/seed.js.
Seed.print(JSON.stringify(method));