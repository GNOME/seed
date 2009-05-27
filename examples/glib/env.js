#!/usr/bin/env seed
GLib = imports.gi.GLib;

print(GLib.getenv("HOME"));
GLib.setenv("SEED", "Why Hello!");
print(GLib.getenv("SEED"));
GLib.unsetenv("SEED");
