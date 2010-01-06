#!../../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk

HelloWindowType = {
    parent: "Mom",
    name: "HelloWindow"
}

try
{
    HelloWindow = new GType(HelloWindowType)
}
catch (e)
{
    testsuite.assert(e.message == "GType constructor expected GType for parent")
}
