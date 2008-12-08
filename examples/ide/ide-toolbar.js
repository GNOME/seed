IDEToolbarType = {
    parent: Gtk.Toolbar.type,
    name: "IDEToolbar",
    instance_init: function(klass)
    {
        this.new_button = actions.get_action("new").create_tool_item();
        this.open_button = actions.get_action("open").create_tool_item();
        this.save_button = actions.get_action("save").create_tool_item();
        this.undo_button = actions.get_action("undo").create_tool_item();
        this.redo_button = actions.get_action("redo").create_tool_item();
        this.execute_button = actions.get_action("execute").create_tool_item();
        
        this.insert(this.new_button, -1);
        this.insert(this.open_button, -1);
        this.insert(this.save_button, -1);
        this.insert(new Gtk.SeparatorToolItem(), -1);
        this.insert(this.undo_button, -1);
        this.insert(this.redo_button, -1);
        this.insert(new Gtk.SeparatorToolItem(), -1);
        this.insert(this.execute_button, -1);
        
        this.show_all();
    }};

IDEToolbar = new GType(IDEToolbarType);

