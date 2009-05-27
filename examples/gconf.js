#!/usr/bin/env seed
GConf = imports.gi.GConf;

GConf.init(Seed.argv);

// client = new GConf.Client() makes GConf segfault, don't think it's our fault.
client = GConf.Client.get_default();
value = client.get_string("/apps/gedit-2/preferences/editor/colors/scheme");
print(value);
