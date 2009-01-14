#!/usr/bin/env seed

Seed.import_namespace("Gtk","2.0");
Seed.import_namespace("Vte");

with (Gtk)
{
	init(null, null);

	var window = new Window();
	window.signal.hide.connect(function () { Gtk.main_quit(); } );
	
	var scroll = new ScrolledWindow();

	var vte = new Vte.Terminal();
	vte.fork_command("/bin/bash");
	vte.signal.child_exited.connect(function(){main_quit()});
	vte.signal.window_title_changed.connect
		(function(terminal)
		 {
			 window.set_title(terminal.get_window_title());
		 });


	scroll.add(vte);
	scroll.set_policy(PolicyType.AUTOMATIC,
					  PolicyType.AUTOMATIC);

	window.add(scroll);
	window.resize(600,400);
	window.show_all();

	main();

}
