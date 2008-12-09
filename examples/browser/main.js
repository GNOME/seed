#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");

/* Todo (priority order):
	* Ctrl-click-open-in-new-tab?
	* Progress
	* Bookmarks
	* FIND IN PAGE
	* History
	* Search bar
	* Zoom
	* View source (or is web inspector enough? it's nicer than anything I can do)
	* Save / open local files
	* printinglol?
	* cookies?? do they not happen magically?
	* favicon
	* Settings manager
	* Status bar (hover, etc)
*/

// Configuration
var homePage = "http://www.google.com";
var selectTabOnCreation = false;
var webKitSettings = new WebKit.WebSettings({enable_developer_extras: true});

Seed.include("browser-actions.js");
Seed.include("browser-toolbar.js");
Seed.include("browser-tab.js");
Seed.include("browser-main.js");

Gtk.init(null, null);

var window = new Gtk.Window({title: "Browser"});
window.signal.hide.connect(Gtk.main_quit);
window.resize(800,800);
window.add_accel_group(initialize_actions());

var browser = new TabbedBrowser();
browser.newTab().navigateTo(homePage);
window.add(browser);

window.show_all();
Gtk.main();
