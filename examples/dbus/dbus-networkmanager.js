#!/usr/local/bin/seed
DBus = imports.dbus;
GLib = imports.gi.GLib;
NetworkManagerInterfaces = imports.networkManagerIfaces;

function NetworkManager() {
    this._init();
}
NetworkManager.prototype = {
    _init: function() {
	DBus.system.proxifyObject (this, 
				   'org.freedesktop.NetworkManager', 
				   '/org/freedesktop/NetworkManager');
    }
};
DBus.proxifyPrototype (NetworkManager.prototype, 
		       NetworkManagerInterfaces.NetworkManager);

function NetworkManagerDevice(path) {
    this._init(path);
}
NetworkManagerDevice.prototype = {
    _init: function(path) {
	DBus.system.proxifyObject (this, 
				   'org.freedesktop.NetworkManager', 
				   path);
    }
};
DBus.proxifyPrototype (NetworkManagerDevice.prototype, 
		       NetworkManagerInterfaces.NetworkManagerDevice);

function NetworkManagerWirelessDevice(path) {
    this._init(path);
}
NetworkManagerWirelessDevice.prototype = {
    _init: function(path) {
	DBus.system.proxifyObject (this, 
				   'org.freedesktop.NetworkManager', 
				   path);
    }
};
DBus.proxifyPrototype (NetworkManagerWirelessDevice.prototype, 
		       NetworkManagerInterfaces.NetworkManagerDeviceWireless);

function NetworkManagerAccessPoint(path) {
    this._init(path);
}
NetworkManagerAccessPoint.prototype = {
    _init: function(path) {
	DBus.system.proxifyObject (this, 
				   'org.freedesktop.NetworkManager', 
				   path);
    }
};
DBus.proxifyPrototype (NetworkManagerAccessPoint.prototype, 
		       NetworkManagerInterfaces.NetworkManagerAccessPoint);

var nm = new NetworkManager();

function print_access_point (path){
    var ap = new NetworkManagerAccessPoint (path);
    ap.GetAllRemote(function(result){
	Seed.print ("Access point. SSID: " + result.Ssid 
		    + " Strength: " + result.Strength);
    });
}

function handle_points (result){
    for (var i=0; i<result.length; i++){
	print_access_point(result[i]);
    }
}

function print_wireless(path){
    wireless = new NetworkManagerWirelessDevice(path);
    wireless.GetAccessPointsRemote (handle_points);
}

function handle_type (path){
    return function(result){
	if (result == 2)
	    print_wireless(path);
    }
}

nm.GetDevicesRemote(
    function (result, exception){
	for (var i = 0; i<result.length; i++){
	    var dev = new NetworkManagerDevice(result[i]);
	    var type = 
		dev.GetRemote("DeviceType", handle_type (result[i]));
	    }
    });

var mainloop = GLib.main_loop_new();
GLib.main_loop_run(mainloop);


