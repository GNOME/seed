#!/usr/bin/env seed
DBus = imports.dbus;
GLib = imports.gi.GLib;

function ConsoleKitManager() {
    this._init();
}
ConsoleKitManager.prototype = {
    _init: function() {
	DBus.system.proxifyObject (this, 
				   'org.freedesktop.ConsoleKit',
				   '/org/freedesktop/ConsoleKit/Manager');
    }
};

function ConsoleKitSession(path) {
    this._init(path);
}
ConsoleKitSession.prototype = {
    _init: function(path) {
	DBus.system.proxifyObject (this, 
				   'org.freedesktop.ConsoleKit',
				   path);
    }
};

var ManagerIface = {
    name: 'org.freedesktop.ConsoleKit.Manager',
    methods: [
	{ name: 'GetCurrentSession', inSignature: '', outSignature: 'o' },
    ]
};
var SessionIFace = {
    name: 'org.freedesktop.ConsoleKit.Session',
    methods: [
	{ name: 'IsLocal', inSignature: '', outSignature: 'b'}
    ]
};


DBus.proxifyPrototype (ConsoleKitManager.prototype, ManagerIface);
DBus.proxifyPrototype (ConsoleKitSession.prototype, SessionIFace);

manager = new ConsoleKitManager();
manager.GetCurrentSessionRemote(
    function(result, exception){
	session = new ConsoleKitSession(result);
	session.IsLocalRemote (function (result){
	    if (result)
		print ("Session is local");
	    else
		print ("Session is remote");
	    Seed.quit();
	});
    });

mainloop = GLib.main_loop_new();
GLib.main_loop_run(mainloop);
