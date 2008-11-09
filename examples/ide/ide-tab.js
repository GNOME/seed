IDETabType = {
    parent: Gtk.VBox,
    name: "IDETab",
    class_init: function(klass, prototype)
    {
        prototype.create_frame = function (child)
        {
            var scroll = new Gtk.ScrolledWindow({
                                vscrollbar_policy: Gtk.PolicyType.automatic,
                                hscrollbar_policy: Gtk.PolicyType.automatic});
            scroll.add(child);

            var frame = new Gtk.Frame();
            frame.set_shadow_type(1);
            frame.add(scroll);

            return frame;
        }
    },
    instance_init: function(klass)
    {
        this.source_view = new IDESourceView();
        this.source_view.tab = this;
        this.terminal = new Vte.Terminal();
        this.pane = new Gtk.VPaned();
        
        this.pane.add1(this.create_frame(this.source_view));
        this.pane.add2(this.create_frame(this.terminal));
        
        this.pane.set_position(700 * (2/3));
        
        this.pack_start(this.pane, true, true);
    }};

IDETab = new GType(IDETabType);
