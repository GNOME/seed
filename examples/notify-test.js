#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Gio = imports.gi.Gio;
Notify = imports.gi.Notify;

Gtk.init(Seed.argv);

function file_changed(monitor, child, other, event)
{
	if(event != Gio.FileMonitorEvent.CREATED)
		return;

    var notification = new Notify.Notification({
    	summary: "File '" + child.get_basename() + "' Created",
		body : child.get_path()
	});
	
    notification.set_timeout(1000);
    notification.show();
}

print("Monitoring files created in current directory");
Notify.init("Seed Test!");

file = Gio.file_new_for_path(".");
monitor = file.monitor_directory();

monitor.signal.changed.connect(file_changed);

Gtk.main();

