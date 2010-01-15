#!/usr/bin/env seed

testsuite = imports.testsuite
Gio = imports.gi.Gio

try
{
	Gio.simple_write(".", "test")
	testsuite.unreachable()
}
catch (e)
{
	testsuite.assert(e.name == "GIoError")
}
