#!/usr/bin/env seed
GLib = imports.gi.GLib;

timer = GLib.timer_new();

GLib.timer_start(timer);

while (GLib.timer_elapsed(timer, null) < 2)
{
}
print(GLib.timer_elapsed(timer, null));

