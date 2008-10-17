#!/usr/local/bin/seed
Seed.import_namespace("Gdk");
Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");

function title_changed(view, title)
{
    this.title = "SpaceWhale - " + title.title;
}

function key_pressed(view, key_event)
{
    a = key_event.type;
    if (a == Gdk.EventType.key_press)
	Seed.print("Win!\n");
}

function update_progress(view, progress)
{
    this.update(progress/100);
}

function forward(entry)
{
    this.go_forward();
}

function back(entry)
{
    this.go_back();
}

function browse(entry)
{
	if (entry.text.search("://") < 0)
	{
		entry.text = "http://" + entry.text;
	}
	
	this.open(entry.text);
}

function zoom_in(button)
{
    this.zoom_in();
}

function zoom_out(button)
{
    this.zoom_out();
}

function refresh(button)
{
    this.reload();
}

function quit()
{
    Gtk.main_quit();
}

Gtk.init(null, null);
window = new Gtk.Window();
window.signal_hide.connect(quit);
window.title = "SpaceWhale"
vbox = new Gtk.VBox();
hbox = new Gtk.HBox();

back_button = new Gtk.ToolButton();
forward_button = new Gtk.ToolButton();
zoom_in_button = new Gtk.ToolButton();
zoom_out_button = new Gtk.ToolButton();
refresh_button = new Gtk.ToolButton();

back_button.set_stock_id("gtk-go-back");
forward_button.set_stock_id("gtk-go-forward");
zoom_in_button.set_stock_id("gtk-zoom-in");
zoom_out_button.set_stock_id("gtk-zoom-out");
refresh_button.set_stock_id("gtk-refresh");



view = new WebKit.WebView();
view.open("http://www.reddit.com/");
view.full_content_zoom = true;
entry = new Gtk.Entry();
bar = new Gtk.ProgressBar();
bar.text = "SpaceWhale";
view.signal_load_progress_changed.connect(update_progress, bar);
view.signal_key_press_event.connect(key_pressed);
view.set_scroll_adjustments(null,null);



entry.signal_activate.connect(browse, view);
forward_button.signal_clicked.connect(forward, view);
back_button.signal_clicked.connect(back, view);
zoom_in_button.signal_clicked.connect(zoom_in, view);
zoom_out_button.signal_clicked.connect(zoom_out, view);
refresh_button.signal_clicked.connect(refresh, view);
view.signal_title_changed.connect(title_changed, window);

hbox.pack_start(back_button);
hbox.pack_start(forward_button);
hbox.pack_start(entry, true, true);
hbox.pack_start(zoom_in_button);
hbox.pack_start(zoom_out_button);
hbox.pack_start(refresh_button);

vbox.pack_start(hbox, false, true);
vbox.pack_start(view, true, true);
vbox.pack_start(bar);

window.resize(800,900);

window.add(vbox);
window.show_all();
Gtk.main();
