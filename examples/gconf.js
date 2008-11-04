#!/usr/local/bin/seed
Seed.import_namespace("GConf");

GConf.init(null, null);

client = GConf.client_get_default();
value = client.get_string("/apps/gedit-2/preferences/editor/colors/scheme");
Seed.print(value);
