#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");

/* Todo (priority order):
	* Key combos (new, close, Ctrl-L, etc)
	* Ctrl-click-open-in-new-tab?
	* Progress
	* Bookmarks
	* FIND IN PAGE
	* History
	* Search bar
	* Zoom
	* View source (or is web inspector enough? it's nicer than anything I can do)
	* Save / open local files
	* printinglol?
	* cookies?? do they not happen magically?
	* favicon
	* Settings manager
	* Status bar (hover, etc)
*/

// Configuration
var homePage = "http://www.google.com";
var selectTabOnCreation = false;
var webKitSettings = new WebKit.WebSettings({enable_developer_extras: true});

// Actions
function initialize_actions()
{
	actions = new Gtk.ActionGroup({name:"toolbar"});

	accels = new Gtk.AccelGroup();

	var new_tab_action = new Gtk.Action({name:"new", label:"New Tab",
		                                 tooltip:"New Tab", stock_id:"gtk-new"});
	new_tab_action.set_accel_group(accels);
	actions.add_action_with_accel(new_tab_action, "<Control>t");
	new_tab_action.connect_accelerator();
	new_tab_action.signal.activate.connect(
		function ()
		{
			browser.newTab().navigateTo(homePage);
			browser.page = browser.get_n_pages() - 1;
		}
	);

	var close_tab_action = new Gtk.Action({name:"close", label:"Close",
		                                   tooltip:"Close Tab", stock_id:"gtk-close"});
	close_tab_action.set_accel_group(accels);
	actions.add_action_with_accel(close_tab_action, "<Control>w");
	close_tab_action.connect_accelerator();
	close_tab_action.signal.activate.connect(close_tab);
	
	return accels;
}

function current_tab()
{
	return browser.get_nth_page(browser.page);
}

function go_back()
{
	current_tab().webView.go_back();
	return false;
}

function go_forward()
{
	current_tab().webView.go_forward();
	return false;
}

function refresh_page()
{
	current_tab().webView.reload();
	return false;
}

function close_tab()
{
	browser.remove_page(browser.page_num(current_tab()));

	if(!browser.get_n_pages())
	{
		browser.newTab().navigateTo(homePage);
	}
	
	return false;
}

function title_changed(webView, webFrame, title, tab)
{
	if(title.length > 25)
		title = title.slice(0,25) + "...";
	
	tab.titleLabel.label = title;
	return false;
}

function url_changed(webView, webFrame, tab)
{
	tab.toolbar.urlBar.text = webFrame.get_uri();
	return false;
}

function newTab_requested(webView, webFrame, newWebView)
{
	newWebView = new WebKit.WebView();
	newWebView.signal.web_view_ready.connect(newTab_ready);
	return newWebView;
}

function newTab_ready(webView)
{
	browser.newTab().setWebView(webView);	
	return false;
}

function browse(urlBar, tab)
{
	if (urlBar.text.search("://") < 0)
	{
		urlBar.text = "http://" + urlBar.text;
	}
	
	tab.webView.open(urlBar.text);
	
	return false;
}

BrowserToolbarType = {
	parent: Gtk.HBox.type,
	name: "BrowserToolbar",
	class_init: function(klass, prototype)
	{
	},
	instance_init: function(klass)
	{
		this.urlBar = new Gtk.Entry();
		
		var back = new Gtk.ToolButton({stock_id:"gtk-go-back"});
		var forward = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
		var refresh = new Gtk.ToolButton({stock_id:"gtk-refresh"});
		
		back.signal.clicked.connect(go_back);
		forward.signal.clicked.connect(go_forward);
		refresh.signal.clicked.connect(refresh_page);
	
		this.pack_start(back);
		this.pack_start(forward);
		this.pack_start(refresh);
		this.pack_start(this.urlBar, true, true);
	}};

BrowserToolbar = new GType(BrowserToolbarType);

BrowserTabType = {
	parent: Gtk.VBox.type,
	name: "BrowserTab",
	class_init: function(klass, prototype)
	{
		prototype.navigateTo = function (location)
		{
			if(!this.webView)
				this.setWebView(new WebKit.WebView());
			
			this.webView.open(location);
			this.show_all();
		}
		
		prototype.setWebView = function (webView)
		{
			if(this.webView)
				return;
			
			this.webView = webView;
			
			this.webView.set_scroll_adjustments(null, null);
			this.webView.signal.title_changed.connect(title_changed, null, this);
			this.webView.signal.load_committed.connect(url_changed, null, this);
			this.webView.signal.create_web_view.connect(newTab_requested, null, this);
			this.webView.signal.web_view_ready.connect(newTab_ready, null, this);
			
			this.toolbar.urlBar.signal.activate.connect(browse, null, this);
			
			webView.set_settings(webKitSettings);
			var inspector = webView.get_inspector();

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
			
			
			s = new Gtk.ScrolledWindow();
			s.add(this.webView);
			this.pack_start(s, true, true);
			this.show_all();
		}
	},
	instance_init: function(klass)
	{
		this.webView = null;
		
		this.toolbar = new BrowserToolbar();

		this.pack_start(this.toolbar);
	
		var closeButton = new Gtk.Button();
		closeButton.set_image(new Gtk.Image({stock: "gtk-close", 
				icon_size: Gtk.IconSize.menu}));
		closeButton.signal.clicked.connect(close_tab, this);
		closeButton.set_relief(Gtk.ReliefStyle.none);
	
		this.title = new Gtk.HBox();
		this.titleLabel = new Gtk.Label({label:"Untitled"})
		this.title.pack_start(this.titleLabel);
		this.title.pack_start(closeButton);
		this.title.show_all();
		
		this.show_all();
	}};

BrowserTab = new GType(BrowserTabType);

TabbedBrowserType = {
	parent: Gtk.Notebook.type,
	name: "TabbedBrowser",
	class_init: function(klass, prototype)
	{
		prototype.newTab = function ()
		{
			var tab = new BrowserTab();
			
			this.append_page(tab, tab.title);
			this.set_tab_reorderable(tab, true);
			
			if(selectTabOnCreation)
				this.page = this.get_n_pages() - 1;
			
			return tab;
		}
	},
	instance_init: function(klass)
	{
		
	}};

TabbedBrowser = new GType(TabbedBrowserType);

Gtk.init(null, null);

var window = new Gtk.Window({title: "Browser"});
window.signal.hide.connect(Gtk.main_quit);
window.resize(800,800);
window.add_accel_group(initialize_actions());

var browser = new TabbedBrowser();
browser.newTab().navigateTo(homePage);
window.add(browser);

window.show_all();
Gtk.main();
