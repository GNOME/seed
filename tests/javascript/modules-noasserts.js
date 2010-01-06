#!../../src/seed

// This test is merely to appease the coverage checker (calls init in each
// module); it will also catch unresolved symbol errors in the modules.

try
{
	cairo = imports.cairo;
	canvas = imports.canvas;
	dbus = imports.dbus;
	example = imports.example;
	gettext = imports.gettext;
	gtkbuilder = imports.gtkbuilder;
	libxml = imports.libxml;
	mpfr = imports.mpfr;
	Multiprocessing = imports.Multiprocessing;
	os = imports.os;
	readline = imports.readline;
	sandbox = imports.sandbox;
	sqlite = imports.sqlite;
}
catch(e)
{

}
