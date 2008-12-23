#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Gtk.init(null, null);

BrowserToolbarType = {
    parent: Gtk.HBox.type,
    name: "BrowserToolbar",
    instance_init: function(klass)
    {
        this.urlBar = new Gtk.Entry();

        this.back = new Gtk.ToolButton({stock_id:"gtk-go-back"});
        this.forward = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
        this.refresh = new Gtk.ToolButton({stock_id:"gtk-refresh"});

        this.back.signal.clicked.connect(back);
        this.forward.signal.clicked.connect(forward);
        this.refresh.signal.clicked.connect(refresh);
        this.urlBar.signal.activate.connect(browse);

        this.pack_start(this.back);
        this.pack_start(this.forward);
        this.pack_start(this.refresh);

        this.pack_start(this.urlBar, true, true);
    }};
BrowserToolbar = new GType(BrowserToolbarType);

function forward(button)
{
    Seed.print("forward");
}

function back(button)
{
    Seed.print("back");
}

function refresh(button)
{
    Seed.print("refresh");
}

function browse(button)
{
    Seed.print("browse");
}

window = new Gtk.Window({title: "Browser"});
window.signal.hide.connect(function () { Gtk.main_quit() });
window.add(new BrowserToolbar());
window.show_all();

Gtk.main();
