#!/usr/bin/env seed

GLib = imports.gi.GLib;

count = 0;

function test(){
	count++;
	print("Hello from timeout number " + count);
	if (count == 5)
		Seed.quit();
	return true;
}

GLib.timeout_add(0, 500, test);

// No offset/size data for GLib.MainLoop, not our fault. Have to use context right now, because mainloop has a bad typelib info.
context = GLib.main_context_default();
while (1)
	GLib.main_context_iteration();
