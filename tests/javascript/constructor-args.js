#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

try
{
	w = new Gtk.Window();
}
catch (e)
{
	testsuite.unreachable()
}

try
{
	w = new Gtk.Window(1, 2);
	testsuite.unreachable()
}
catch (e)
{
	testsuite.assert(e.name == "ArgumentError") 
}

try
{
	w = new Gtk.Window("safA");
	testsuite.unreachable()
}
catch (e)
{
	testsuite.assert(e.name == "ArgumentError")
}

testsuite.checkAsserts(2)
