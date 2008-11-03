#!/usr/local/bin/seed

Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");
Gtk.init(null, null);

var window = new Gtk.Window({title: "Browser"});
window.resize(600,600);

function quit()
{
    Gtk.main_quit();
}

window.signal_hide.connect(quit);

function create_ui()
{
    var main_ui = new Gtk.VBox();
    var toolbar = new Gtk.HBox();
    
    var browser = new WebKit.WebView();

    var back_button = new Gtk.ToolButton({stock_id: "gtk-go-back"});
    var forward_button = new Gtk.ToolButton({stock_id: "gtk-go-forward"});
    var refresh_button = new Gtk.ToolButton({stock_id: "gtk-refresh"});

    var url_entry = new Gtk.Entry();

    back_button.signal_clicked.connect(back, browser);
    forward_button.signal_clicked.connect(forward, browser);
    refresh_button.signal_clicked.connect(refresh, browser);

    url_entry.signal_activate.connect(browse, browser);

    toolbar.pack_start(back_button);
    toolbar.pack_start(forward_button);
    toolbar.pack_start(refresh_button);
    toolbar.pack_start(url_entry, true, true);

    main_ui.pack_start(toolbar);
    main_ui.pack_start(browser, true, true);
    return main_ui;
}

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
    Seed.print("browser");
}

window.add(create_ui());
window.show_all();

Gtk.main();
