#!/usr/bin/env seed
Seed.import_namespace("GLib");

count = 0;

GLib.timeout_add(500,
function()
{
	count++;
	Seed.print("Hello from timeout number " + count)
	if (count == 5)
		Seed.quit();
	return true;
},0);

// No offset/size data for GLib.MainLoop, not our fault. Have to use context right now, because mainloop has a bad typelib info.
context = GLib.main_context_default();
while (1)
	GLib.main_context_iteration();
