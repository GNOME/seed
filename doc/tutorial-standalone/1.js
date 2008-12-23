#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Gtk.init(null, null);

BrowserToolbarType = {
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
};
BrowserToolbar = new GType(BrowserToolbarType);

window = new Gtk.Window({title: "Browser"});
window.signal.hide.connect(function () { Gtk.main_quit() });
window.add(new BrowserToolbar());
window.show_all();

Gtk.main();
