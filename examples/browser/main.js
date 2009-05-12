#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
WebKit = imports.gi.WebKit;

Gtk.init(Seed.argv);

TabbedBrowser = imports.TabbedBrowser;
BrowserSettings = imports.BrowserSettings;

window = new Gtk.Window({title: "Browser"});
window.resize(800, 600);
window.signal.hide.connect(Gtk.main_quit);

TabbedBrowser.browser = new TabbedBrowser.TabbedBrowser();
window.add(TabbedBrowser.browser);
window.show_all();

Gtk.main();
