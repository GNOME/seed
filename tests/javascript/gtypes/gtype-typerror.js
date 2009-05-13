#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:GType constructor expected GType for parent
// STDERR:

Gtk = imports.gi.Gtk;


HelloWindowType = {
    parent: "Mom",
    name: "HelloWindow"
};

try{
    HelloWindow = new GType(HelloWindowType);
} catch (e) {Seed.print(e.message);}
