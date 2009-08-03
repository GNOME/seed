#!/usr/bin/env seed
GObject = imports.gi.GObject;
ffi = imports.ffi;

signatures = {
	"XOpenDisplay": {arguments: [GObject.TYPE_STRING],
			 returns: GObject.TYPE_POINTER},
	"XDefaultScreen": {arguments: [GObject.TYPE_POINTER],
			   returns: GObject.TYPE_INT}
};

var XLib = new ffi.Library("/usr/lib/libX11.so");

var XOpenDisplay = XLib.XOpenDisplay;
var XDefaultScreen = XLib.XDefaultScreen;

for (var i in signatures){
	XLib[i].signature = signatures[i];
}
	      
display = XOpenDisplay(":0.0");
print(display);

print(XDefaultScreen(display));
