#!/usr/local/bin/seed

Seed.import_namespace("Notify");
Notify.init("Seed Test!");
var notification = new Notify.Notification(
	{"summary" : "I am testling libnotify",
	"body" : "And it seems to be working!"});
notification.set_timeout(1000);
notification.show(null);
