#!/usr/bin/env seed
GObject = imports.gi.GObject;
ffi = imports.ffi;

var app = new ffi.Library();
var strcmp = app.strcmp;

strcmp.signature = {arguments: [GObject.TYPE_STRING, GObject.TYPE_STRING], 
		    returns: GObject.TYPE_INT};

print(strcmp("Hi", "Hi"));
print(strcmp("Boo", "Foo"));
print(strcmp("Foo", "Boo"));
