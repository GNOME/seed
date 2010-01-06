#!../../src/seed

// TODO: this test doesn't actually test anything...

Gtk = imports.gi.Gtk

Gtk.init(Seed.argv)

for(var i = 0; i < 100; i++)
{
    var button = new Gtk.Button({label: "Test!"})
}

