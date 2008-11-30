#!/usr/local/bin/seed
Seed.import_namespace("GLib");

Seed.print(GLib.getenv("HOME"));
GLib.setenv("SEED", "Why Hello!");
Seed.print(GLib.getenv("SEED"));
GLib.unsetenv("SEED");
