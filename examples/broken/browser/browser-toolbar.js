BrowserToolbarType = {
	parent: Gtk.HBox.type,
	name: "BrowserToolbar",
	instance_init: function(klass)
	{
		this.urlBar = new Gtk.Entry();
		this.searchBar = new Gtk.Entry();
	
		this.back = new Gtk.ToolButton({stock_id:"gtk-go-back"});
		this.forward = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
		this.refresh = new Gtk.ToolButton({stock_id:"gtk-refresh"});
	
		this.back.signal.clicked.connect(go_back);
		this.forward.signal.clicked.connect(go_forward);
		this.refresh.signal.clicked.connect(refresh_page);

		this.pack_start(this.back);
		this.pack_start(this.forward);
		this.pack_start(this.refresh);

		this.pack_start(this.urlBar, true, true);
		this.pack_start(this.searchBar, false, false, 5);
	}};

BrowserToolbar = new GType(BrowserToolbarType);
