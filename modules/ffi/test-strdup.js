#!/usr/bin/env seed
GObject = imports.gi.GObject;
ffi = imports.ffi;

var app = new ffi.Library();
var strdup = app.strdup;

strdup.signature = {arguments: [GObject.TYPE_STRING],
		    returns: GObject.TYPE_STRING};

print(strdup("Hi"));

