#!../../../src/seed

testsuite = imports.testsuite
GConf = imports.gi.GConf

GConf.init(Seed.argv)

client = GConf.Client.get_default()

testsuite.assert(client != undefined)
