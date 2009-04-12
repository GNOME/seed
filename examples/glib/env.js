#!/usr/bin/env seed
GLib = imports.gi.GLib;

Seed.print(GLib.getenv("HOME"));
GLib.setenv("SEED", "Why Hello!");
Seed.print(GLib.getenv("SEED"));
GLib.unsetenv("SEED");
