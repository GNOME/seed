#!../../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

w = new Gtk.Window();

try
{
    w.signal.map.connect(3);
    testsuite.unreachable()
}
catch (e)
{
    testsuite.assert(e.message == "Signal connection requires a function as first argument")
}

testsuite.checkAsserts(1)
