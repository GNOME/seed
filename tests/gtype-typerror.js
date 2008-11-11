#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:GType constructor expected Gype for parent
// STDERR:
Seed.import_namespace("Gtk");


HelloWindowType = {
    parent: "Mom",
    name: "HelloWindow"
};

try{
    HelloWindow = new GType(HelloWindowType);
} catch (e) {Seed.print(e.message);}
