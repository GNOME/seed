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

function new_tab_requested(webView, webFrame, newWebView)
{
	newWebView = new WebKit.WebView();
	newWebView.signal.web_view_ready.connect(new_tab_ready);
	return newWebView;
}

function new_tab_ready(webView)
{
	browser.newTab().setWebView(webView);	
	return false;
}

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
			this.webView.signal.create_web_view.connect(new_tab_requested, null, this);
			this.webView.signal.web_view_ready.connect(new_tab_ready, null, this);
			
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
			
			
			var scrollView = new Gtk.ScrolledWindow();
			scrollView.add(this.webView);
			scrollView.set_policy(Gtk.PolicyType.automatic,
								  Gtk.PolicyType.automatic);
			this.pack_start(scrollView, true, true);
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
	
		this.progress = new Gtk.ProgressBar({fraction: 0.5});
	
		this.title = new Gtk.HBox();
		this.titleLabel = new Gtk.Label({label:"Untitled"})
		this.title.pack_start(this.titleLabel);
		this.title.pack_start(closeButton);
		this.title.show_all();
		
		this.show_all();
	}};

BrowserTab = new GType(BrowserTabType);
