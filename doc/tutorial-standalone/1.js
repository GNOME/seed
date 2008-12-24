#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Gtk.init(null, null);

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
            Seed.print("back");
        };

        var forward = function ()
        {
            Seed.print("forward");
        };

        var refresh = function ()
        {
            Seed.print("refresh");
        };

        var browse = function ()
        {
            Seed.print("browse");
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
window.add(new BrowserToolbar());
window.show_all();

Gtk.main();
