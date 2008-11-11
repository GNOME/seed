IDEWindowType = {
    parent: Gtk.Window.type,
    name: "IDEWindow",
    class_init: function(klass, prototype)
    {
		prototype.update_window_title = function (tab)
		{
			var new_filename = tab.header.label.label;
			
			if(new_filename != "")
				this.title = "Seed IDE - " + new_filename;
			else
				this.title = "Seed IDE";
		}
    },
    instance_init: function(klass)
    {
		this.signal.hide.connect(Gtk.main_quit);
		
		this.add_accel_group(init_actions());
		
		this.toolbar = new IDEToolbar();

		this.tab_view = new IDETabView();

		this.vbox = new Gtk.VBox();
		this.vbox.pack_start(this.toolbar);
		this.vbox.pack_start(this.tab_view, true, true);
		this.vbox.show();

		this.add(this.vbox);
		this.resize(700, 700);
		
		this.tab_view.create_tab("../ls.js");
		
		this.show();
    }};

IDEWindow = new GType(IDEWindowType);

