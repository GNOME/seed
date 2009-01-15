#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");

Gtk.init(null, null);

Seed.include("BrowserSettings.js");
Seed.include("BrowserView.js");
Seed.include("BrowserTab.js");
Seed.include("TabbedBrowser.js");
Seed.include("BrowserToolbar.js");

window = new Gtk.Window({title: "Browser"});
window.resize(800, 600);
window.signal.hide.connect(function () { Gtk.main_quit(); });

browser = new TabbedBrowser();
window.add(browser);
window.show_all();

Gtk.main();
