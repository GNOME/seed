#!/usr/bin/env seed
Seed.import_namespace("Gtk");
Seed.import_namespace("Gio");
Seed.import_namespace("Notify");

Gtk.init(null, null);

function file_changed(monitor, child, other, event)
{
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

