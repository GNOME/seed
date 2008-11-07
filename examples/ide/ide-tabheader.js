IDETabHeaderType = {
    parent: Gtk.HBox,
    name: "IDETabHeader",
    class_init: function(klass, prototype)
    {
        prototype.set_tab = function (tab)
        {
            this.close_button.signal.clicked.connect(tab_view.close_tab, tab);
        }
    },
    instance_init: function(klass)
    {
        this.close_button = new Gtk.Button();
        this.close_button.set_image(new Gtk.Image({stock: "gtk-close",
                                    icon_size: Gtk.IconSize.menu}));
        this.close_button.set_relief(Gtk.ReliefStyle.none);

        this.label = new Gtk.Label();

        this.pack_start(this.label);
        this.pack_start(this.close_button);
        this.show_all();
    }};

IDETabHeader = new GType(IDETabHeaderType);

