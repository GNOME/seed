#!/usr/bin/env seed
Gtk = imports.gi.Gtk;
Gio = imports.gi.Gio;
Notify = imports.gi.Notify;

Gtk.init(Seed.argv);

function file_changed(monitor, child, other, event){
    var notification =
	new Notify.Notification({summary: "File Notification",
				 body : "It's not clear what notification system this file is providing an example of." });
    notification.set_timeout(5000);
    notification.show();
}

Seed.print("Monitoring files in current directory");
Notify.init("Seed Test!");

file = Gio.file_new_for_path(".");
monitor = file.monitor_directory();

monitor.signal.changed.connect(file_changed);

Gtk.main();


