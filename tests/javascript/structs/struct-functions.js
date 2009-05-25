#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:

GConf = imports.gi.GConf;

GConf.init(Seed.argv);

client = GConf.Client.get_default();
