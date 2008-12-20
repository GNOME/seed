#!/usr/bin/env seed
Seed.import_namespace("GLib");

timer = GLib.timer_new();

GLib.timer_start(timer);

while (GLib.timer_elapsed(timer, null) < 2)
{
}
Seed.print(GLib.timer_elapsed(timer, null));

