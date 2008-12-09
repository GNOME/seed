#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");

var tabs;

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

function title_changed(view, title)
{
	var tab_title = title.title;
	
	if(tab_title.length > 25)
		tab_title = tab_title.slice(0,25) + "...";
	
   this.label = tab_title;
}

function browse(url_entry)
{
	if (url_entry.text.search("://") < 0)
	{
		url_entry.text = "http://" + url_entry.text;
	}
	
	this.open(url_entry.text);
}

function new_tab(browser_view, browser_frame, new_frame)
{
	new_frame = new WebKit.WebView();


	new_frame.signal.web_view_ready.connect(new_tab_ready, this);
	return new_frame;
}

function new_tab_ready(browser_view)
{
	this.create_tab_with_webview(0, browser_view);	
	return true;
}

function url_changed(browser_view, browser_frame)
{
	this.text = browser_frame.get_uri();
	return true;
}

function close_tab(button)
{
    if(tabs.get_n_pages() > 1)
    {
        tabs.remove_page(tabs.page_num(this));
    }
}

function create_toolbar(browser_view)
{
	var toolbar = new Gtk.HBox();
	
	var back_button = new Gtk.ToolButton({stock_id:"gtk-go-back"});
	back_button.signal.clicked.connect(back, browser_view);
	
	var forward_button = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
	forward_button.signal.clicked.connect(forward, browser_view);
	
	var refresh_button = new Gtk.ToolButton({stock_id:"gtk-refresh"});
	refresh_button.signal.clicked.connect(refresh, browser_view);
	
	toolbar.pack_start(back_button);
	toolbar.pack_start(forward_button);
	toolbar.pack_start(refresh_button);
	
	return toolbar;
}

function create_tab(loc)
{
	var browser_view = new WebKit.WebView();

	settings = new WebKit.WebSettings({enable_developer_extras: true});
	browser_view.set_settings(settings);

	var inspector = browser_view.get_inspector();

	inspector.signal.inspect_web_view.connect(
		function()
		{
			w = new Gtk.Window();
			s = new Gtk.ScrolledWindow();
			w.set_title("Inspector");
			
			w.set_default_size(400, 300);

			view = new WebKit.WebView();
			s.add(view);
			w.add(s);
			
			w.show_all();
			
			return view;
		}
		);
	
	
	return create_tab_with_webview(loc, browser_view);
}

function create_tab_with_webview(loc, browser_view)
{
	var tab = new Gtk.VBox();
	
	var browser_title = new Gtk.Label({label:"Untitled"});
	
	var url_entry = new Gtk.Entry();
	url_entry.signal.activate.connect(browse, browser_view);

	browser_view.set_scroll_adjustments(null,null);
	browser_view.signal.title_changed.connect(title_changed, browser_title);
	browser_view.signal.load_committed.connect(url_changed, url_entry);
	browser_view.signal.create_web_view.connect(new_tab, this);
	browser_view.signal.web_view_ready.connect(new_tab_ready, this);
	
	if(loc != 0)
		browser_view.open(loc);
	
	var toolbar = create_toolbar(browser_view);
	toolbar.pack_start(url_entry, true, true);
	
	tab.pack_start(toolbar);
	tab.pack_start(browser_view, true, true);
	
	var close_button = new Gtk.Button();
	close_button.set_image(new Gtk.Image({stock: "gtk-close", 
			icon_size: Gtk.IconSize.menu}));
	close_button.signal.clicked.connect(close_tab, tab);
	close_button.set_relief(Gtk.ReliefStyle.none);
	
	var tab_header = new Gtk.HBox();
	tab_header.pack_start(browser_title);
	tab_header.pack_start(close_button);
	tab_header.show_all();
	
	tabs.append_page(tab, tab_header);
	tabs.set_tab_reorderable(tab, true);
	tabs.show_all();
	
	return tabs;
}

function create_ui()
{
	var vbox = new Gtk.VBox();
	
	tabs = new Gtk.Notebook();
	tabs.create_tab = create_tab;
	tabs.create_tab_with_webview = create_tab_with_webview;
	
	tabs.create_tab("http://www.reddit.com/");
	tabs.create_tab("http://www.google.com/");
	vbox.pack_start(tabs, true, true);
	
	return vbox;
}

function browser_init()
{
	Gtk.init(null, null);

	var window = new Gtk.Window({title: "Browser"});
	window.signal.hide.connect(Gtk.main_quit);
	window.resize(800,800);
	
	window.add(create_ui());
	
	window.show_all();
	Gtk.main();
}

browser_init();

