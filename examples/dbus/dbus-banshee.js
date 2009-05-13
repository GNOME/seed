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
    methods: [
	{ name: 'Open', inSignature: 's', outSignature: '' },
	{ name: 'Close', inSignature: '', outSignature: '' },
	{ name: 'Pause', inSignature: '', outSignature: '' },
	{ name: 'Play', inSignature: '', outSignature: '' },
	{ name: 'TogglePlaying', inSignature: '', outSignature: '' }
    ],
    signals: [
	{ name: 'EventChanged', inSignature: '', outSignature: 'ssd' },
	{ name: 'StateChanged', inSignature: '', outSignature: 's' }
    ],
    properties: [
	{ name: 'CurrentTrack', signature: 'a{sv}', access: 'read' },
	{ name: 'CurrentUri', signature: 's', access: 'read' },
	{ name: 'CurrentState', signature: 's', access: 'read' },
	{ name: 'LastState', signature: 's', access: 'read' },
	{ name: 'Volume', signature: 'q', access: 'readwrite' },
	{ name: 'Position', signature: 'u', access: 'readwrite' },
	{ name: 'CanSeek', signature: 'b', access: 'read' },
	{ name: 'CanPause', signature: 'b', access: 'read' },
	{ name: 'Length', signature: 'u', access: 'read' }
    ]
};


DBus.proxifyPrototype (PlayerEngine.prototype, PlayerEngineIface);

proxy = new PlayerEngine();
proxy.OpenRemote(Seed.argv[2]);
proxy.PlayRemote(function(){
    Seed.print("proxy.PlayRemote returned");
});

proxy.connect("StateChanged", 
	      function(emitter, state){
		  Seed.print("Banshee state changed: " + state);
	      });

mainloop = GLib.main_loop_new();
GLib.main_loop_run(mainloop);
