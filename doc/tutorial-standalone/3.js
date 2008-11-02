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
    browser.open("http://www.gnome.org");

    var back_button = new Gtk.ToolButton({stock_id: "gtk-go-back"});
    var forward_button = new Gtk.ToolButton({stock_id: "gtk-go-forward"});
    var refresh_button = new Gtk.ToolButton({stock_id: "gtk-refresh"});

    var url_entry = new Gtk.Entry();

    back_button.signal_clicked.connect(back, browser);
    forward_button.signal_clicked.connect(forward, browser);
    refresh_button.signal_clicked.connect(refresh, browser);

    url_entry.signal_activate.connect(browse, browser);
    browser.signal_load_committed.connect(url_changed, url_entry);

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
	this.go_forward();
}

function back(button)
{
	this.go_back();
}

function refresh(button)
{
	this.reload();
}

function browse(url)
{
	if (url.text.search("://") < 0)
	{
		url.text = "http://" + url.text;
	}
	
	this.open(url.text);
}

function url_changed(browser, frame)
{
    this.text = frame.get_uri();
}

window.add(create_ui());
window.show_all();

Gtk.main();
