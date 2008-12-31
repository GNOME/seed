#!/usr/bin/env seed

// JSON.stringify and JSON.fromString come from json2.js available on json.org
// Also in extensions/seed.js.

Seed.import_namespace("Gtk");

var proto = Gtk.Window.prototype;
var method = Seed.introspect(proto.translate_coordinates);

Seed.print(JSON.stringify(method));
