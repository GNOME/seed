#!/usr/local/bin/seed
Seed.import_namespace("Gtk");
Seed.import_namespace("Gio");
Seed.import_namespace("Notify");

Gtk.init(null, null);

function file_changed(monitor, child, other, event)
{
    var notification = 
	new Notify.Notification({"summary": "File Notification",
				 "body" : "This is a poor libnotify example" });
    notification.set_timeout(5000);
    notification.show();
}

Notify.init("Seed Test!");

file = Gio.file_new_for_path(".");
monitor = file.monitor_directory();

monitor.signal_changed.connect(file_changed);

Gtk.main();

