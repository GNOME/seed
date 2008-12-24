#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");
Gtk.init(null, null);

BrowserTab = new GType({
    parent: Gtk.VBox.type,
    name: "BrowserTab",
    instance_init: function(klass)
    {
        var toolbar = new BrowserToolbar();
        var webView = new BrowserView();
        
        this.pack_start(toolbar);
        this.pack_start(webView, true, true);
    }
});

BrowserView = new GType({
    parent: WebKit.WebView.type,
    name: "BrowserView",
    instance_init: function(klass)
    {
        
    }
});

BrowserToolbar = new GType({
    parent: Gtk.HBox.type,
    name: "BrowserToolbar",
    instance_init: function(klass)
    {
        var urlBar = new Gtk.Entry();

        var backButton = new Gtk.ToolButton({stock_id:"gtk-go-back"});
        var forwardButton = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
        var refreshButton = new Gtk.ToolButton({stock_id:"gtk-refresh"});
        
        var back = function ()
        {
            Seed.print("back");
        }
        
        var forward = function ()
        {
            Seed.print("forward");
        }
        
        var refresh = function ()
        {
            Seed.print("refresh");
        }
        
        var browse = function ()
        {
            Seed.print("browse");
        }

        backButton.signal.clicked.connect(back);
        forwardButton.signal.clicked.connect(forward);
        refreshButton.signal.clicked.connect(refresh);
        urlBar.signal.activate.connect(browse);

        this.pack_start(backButton);
        this.pack_start(forwardButton);
        this.pack_start(refreshButton);

        this.pack_start(urlBar, true, true);
    }
});

window = new Gtk.Window({title: "Browser"});
window.signal.hide.connect(function () { Gtk.main_quit() });
window.add(new BrowserTab());
window.show_all();

Gtk.main();
