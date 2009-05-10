#!/usr/local/bin/seed
DBus = imports.dbus;
GLib = imports.gi.GLib;

function PlayerEngine() {
    this._init();
}

PlayerEngine.prototype = {
    _init: function() {
	DBus.session.proxifyObject (this, 'org.bansheeproject.Banshee','/org/bansheeproject/Banshee/PlayerEngine');
    }
};

var PlayerEngineIface = {
    name: 'org.bansheeproject.Banshee.PlayerEngine',
    methods: [{name:'Play', inSignature: ''},
	      {name:'Open', inSignature: 's'}],
};

DBus.proxifyPrototype (PlayerEngine.prototype, PlayerEngineIface);

proxy = new PlayerEngine();
proxy.OpenRemote(Seed.argv[2]);
proxy.PlayRemote(function(){
    Seed.print("proxy.PlayRemote returned");
    Seed.quit();
});

mainloop = GLib.main_loop_new();
GLib.main_loop_run(mainloop);
