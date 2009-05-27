Gtk = imports.gi.Gtk;

var gtkMain = Gtk.main;
Gtk.main = function() {};
function assertEquals(expected, value, message) {
    if (expected != value) {
	throw new Error('FAILED: ' + message + ', expected: ' + expected + ' got: ' + value);
    } else {
	print('PASS: ' + message);
    }
}

pad = imports.main;

var contexts = [];
var sandboxContext = pad.sandbox.Context;
pad.sandbox.Context = function() {
    contexts.push(this);   
};
pad.sandbox.Context.prototype = {
    add_globals: function() {},
    destroy: function() {},
    eval: function() {}
};

pad.execute_file();
assertEquals(1, contexts.length, 'execute file should create only one context');
