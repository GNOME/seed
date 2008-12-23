#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Gtk.init(null, null);

BrowserToolbarType = {
    parent: Gtk.HBox.type,
    name: "BrowserToolbar",
    instance_init: function(klass)
    {
        var urlBar = new Gtk.Entry();

        var back = new Gtk.ToolButton({stock_id:"gtk-go-back"});
        var forward = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
        var refresh = new Gtk.ToolButton({stock_id:"gtk-refresh"});

        back.signal.clicked.connect(function ()
        {
            Seed.print("back");
        });
        
        forward.signal.clicked.connect(function ()
        {
            Seed.print("forward");
        });
        
        refresh.signal.clicked.connect(function ()
        {
            Seed.print("refresh");
        });
        
        urlBar.signal.activate.connect(function ()
        {
            Seed.print("browse");
        });

        this.pack_start(back);
        this.pack_start(forward);
        this.pack_start(refresh);

        this.pack_start(urlBar, true, true);
    }
};
BrowserToolbar = new GType(BrowserToolbarType);

window = new Gtk.Window({title: "Browser"});
window.signal.hide.connect(function () { Gtk.main_quit() });
window.add(new BrowserToolbar());
window.show_all();

Gtk.main();
