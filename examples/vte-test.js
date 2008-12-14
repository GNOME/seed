#!/usr/bin/env seed
Seed.import_namespace("Gtk","2.0");
Seed.import_namespace("Vte");

with (Gtk)
{
	init(null, null);

	var window = new Window();
	var scroll = new ScrolledWindow();

	var vte = new Vte.Terminal();
	vte.fork_command("/bin/bash");
	vte.signal.child_exited.connect(main_quit);
	vte.signal.window_title_changed.connect
		(function(terminal)
		 {
			 window.set_title(terminal.get_window_title());
		 });


	scroll.add(vte);
	scroll.set_policy(PolicyType.automatic,
					  PolicyType.automatic);

	window.add(scroll);
	window.show_all();

	window.width_request = 500;
	window.height_request = 400;

	main();

}