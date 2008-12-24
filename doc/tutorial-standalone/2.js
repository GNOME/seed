#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");
Gtk.init(null, null);

TabbedBrowser = new GType({
    parent: Gtk.Notebook.type,
    name: "TabbedBrowser",
    instance_init: function(klass)
    {
	this.new_tab = function (url)
	{
	    this.append_page(new BrowserTab(), new Gtk.Label({label:"hello"}));
	};

	this.current_tab = function ()
	{
	    return this.get_nth_page(this.page);
	};

	this.new_tab("http://www.google.com");
    }
});

BrowserTab = new GType({
    parent: Gtk.VBox.type,
    name: "BrowserTab",
    instance_init: function(klass)
    {
	var toolbar = new BrowserToolbar();
	var web_view = new BrowserView();

	this.pack_start(toolbar);
	this.pack_start(web_view, true, true);

	this.get_web_view = function()
	{
	    return web_view;
	};
    }
});

BrowserView = new GType({
    parent: WebKit.WebView.type,
    name: "BrowserView",
    instance_init: function(klass)
    {
	this.browse = function (url)
	{
	    if(url.search("://") < 0)
		url = "http://" + url;

	    this.open(url);
	};
    }
});

BrowserToolbar = new GType({
    parent: Gtk.HBox.type,
    name: "BrowserToolbar",
    instance_init: function(klass)
    {
	var url_bar = new Gtk.Entry();

	var back_button = new Gtk.ToolButton({stock_id:"gtk-go-back"});
	var forward_button = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
	var refresh_button = new Gtk.ToolButton({stock_id:"gtk-refresh"});

	var back = function ()
	{
	    browser.current_tab().get_web_view().go_back();
	};

	var forward = function ()
	{
	    browser.current_tab().get_web_view().go_forward();
	};

	var refresh = function ()
	{
	    browser.current_tab().get_web_view().reload();
	};

	var browse = function (url)
	{
	    browser.current_tab().get_web_view().browse(url.text);
	};

	back_button.signal.clicked.connect(back);
	forward_button.signal.clicked.connect(forward);
	refresh_button.signal.clicked.connect(refresh);
	url_bar.signal.activate.connect(browse);

	this.pack_start(back_button);
	this.pack_start(forward_button);
	this.pack_start(refresh_button);

	this.pack_start(url_bar, true, true);
    }
});

window = new Gtk.Window({title: "Browser"});
window.signal.hide.connect(function () { Gtk.main_quit(); });

browser = new TabbedBrowser();
window.add(browser);
window.show_all();

Gtk.main();
