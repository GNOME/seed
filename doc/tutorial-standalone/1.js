#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Gtk.init(null, null);

BrowserToolbar = new GType({
    parent: Gtk.HBox.type,
    name: "BrowserToolbar",
    init: function (){
        // Private
        var url_bar = new Gtk.Entry();

        var back_button = new Gtk.ToolButton({stock_id:"gtk-go-back"});
        var forward_button = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
        var refresh_button = new Gtk.ToolButton({stock_id:"gtk-refresh"});

        var back = function (){
            Seed.print("Go Back");
        };

        var forward = function (){
            Seed.print("Go Forward");
        };

        var refresh = function (){
            Seed.print("Refresh");
        };

        var browse = function (url){
            Seed.print("Navigate to: " + url.text);
        };

        // Implementation
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
window.signal.hide.connect(Gtk.main_quit);

toolbar = new BrowserToolbar();
window.add(toolbar);
window.show_all();

Gtk.main();
