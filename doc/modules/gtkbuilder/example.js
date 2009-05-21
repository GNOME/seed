<?xml version="1.0"?>
<programlisting>
#!/usr/local/bin/seed
Gtk = imports.gi.Gtk;
GtkBuilder = imports.gtkbuilder;

handlers = {
    ok_button_clicked: function(button){
	Seed.quit();
    }
};

Gtk.init(Seed.argv);

b = new Gtk.Builder();
b.add_from_file("test.ui");
b.connect_signals(handlers);

d = b.get_object("dialog1");

d.show_all();

Gtk.main();
</programlisting>
