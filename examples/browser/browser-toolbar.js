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
