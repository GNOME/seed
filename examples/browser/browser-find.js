// TODO: escape in the text field should hide it, not search more!

FindBoxType = {
	parent: Gtk.HBox.type,
	name: "FindBox",
	instance_init: function(klass)
	{
		var closeButton = new Gtk.Button();
		closeButton.set_image(new Gtk.Image({stock: "gtk-close", 
				icon_size: Gtk.IconSize.Menu}));
		closeButton.signal.clicked.connect(
			function ()
            {
				this.entryBox.text = "";
				this.hide();
            }, this);
		closeButton.set_relief(Gtk.ReliefStyle.None);

		var findLabel = new Gtk.Label({label:"Find: "});
		this.entryBox = new Gtk.Entry();
		this.entryBox.signal.key_release_event.connect(find, this);

		this.caseSensitive = new Gtk.CheckButton({label:"Match case"});
/*		this.caseSensitive.signal.toggled.connect(
			function (a,b) 
            { 
				// BROKEN.
				current_tab().webView.unmark_text_matches();
				find(this, 0, b); 
				return false; 
				},  this);*/

		this.pack_start(closeButton);
		this.pack_start(findLabel);
		this.pack_start(this.entryBox, true, true);
		this.pack_start(this.caseSensitive, false, false, 10);
	}};

FindBox = new GType(FindBoxType);
