IDETabHeaderType = {
    parent: Gtk.HBox.type,
    name: "IDETabHeader",
    class_init: function(klass, prototype)
    {

    },
    instance_init: function(klass)
    {
    	//var gtkrc = new Gtk.RcStyle();
    	//gtkrc.parse_string("xthickness = 0");
    	
        this.close_button = new Gtk.Button();
        this.close_button.set_image(new Gtk.Image({stock: "gtk-close",
                                    icon_size: Gtk.IconSize.menu}));
        this.close_button.set_relief(Gtk.ReliefStyle.none);
        //this.close_button.modify_style(gtkrc);

        this.label = new Gtk.Label();

        this.pack_start(this.label);
        this.pack_start(this.close_button);
        this.show_all();
    }};

IDETabHeader = new GType(IDETabHeaderType);

