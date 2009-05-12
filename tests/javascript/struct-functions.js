#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:

Seed.import_namespace("GConf");

GConf.init(Seed.argv);

client = GConf.Client.get_default();
