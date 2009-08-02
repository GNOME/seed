#!/usr/bin/env seed
GObject = imports.gi.GObject;
ffi = imports.ffi;

var app = new ffi.Library();
var sin = app.sin;

sin.signature = {arguments: [GObject.TYPE_DOUBLE], 
		 returns: GObject.TYPE_DOUBLE};

print(sin(3.14));

