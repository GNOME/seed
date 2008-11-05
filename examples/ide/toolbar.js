function ide_actions(tab)
{
    this.actions = new Gtk.ActionGroup({name:"toolbar"});
    
    this.accels = new Gtk.AccelGroup();
    window.add_accel_group(this.accels);
    
    this.new_action = new Gtk.Action({name:"new", label:"New",
                                      tooltip:"New File", stock_id:"gtk-new"});
    this.new_action.set_accel_group(this.accels);
    this.actions.add_action_with_accel(this.new_action, "<Control>n");
    this.new_action.connect_accelerator();
    this.new_action.signal_activate.connect(new_file, tab);
    
    this.open_action = new Gtk.Action({name:"open", label:"Open",
                                      tooltip:"Open File", stock_id:"gtk-open"});
    this.open_action.set_accel_group(this.accels);
    this.actions.add_action_with_accel(this.open_action, "<Control>o");
    this.open_action.connect_accelerator();
    this.open_action.signal_activate.connect(open_file, tab);
    
    this.save_action = new Gtk.Action({name:"save", label:"Save",
                                      tooltip:"Save File", stock_id:"gtk-save"});
    this.save_action.set_accel_group(this.accels);
    this.actions.add_action_with_accel(this.save_action, "<Control>s");
    this.save_action.connect_accelerator();
    this.save_action.signal_activate.connect(save_file, tab);
    
    this.undo_action = new Gtk.Action({name:"undo", label:"Undo",
                                      tooltip:"Undo", stock_id:"gtk-undo"});
    this.undo_action.signal_activate.connect(undo, tab);
    
    this.redo_action = new Gtk.Action({name:"redo", label:"Redo",
                                      tooltip:"Redo", stock_id:"gtk-redo"});
    this.redo_action.signal_activate.connect(redo, tab);
    
    this.execute_action = new Gtk.Action({name:"execute", label:"Execute",
                                      tooltip:"Execute File", stock_id:"gtk-execute"});
    this.execute_action.set_accel_group(this.accels);
    this.actions.add_action_with_accel(this.execute_action, "<Control>r");
    this.execute_action.connect_accelerator();
    this.execute_action.signal_activate.connect(execute, tab);
}

function new_file(sv)
{
    var tab = new ide_tab("");
}

function open_file(sv)
{
    var file_chooser = new Gtk.FileChooserDialog();
    var file_filter = new Gtk.FileFilter();
    file_filter.add_mime_type("text/javascript");
    file_chooser.set_filter(file_filter);
    file_chooser.add_button("Cancel", Gtk.ResponseType.cancel);
    file_chooser.add_button("Open", Gtk.ResponseType.accept);
    file_chooser.set_action(Gtk.FileChooserAction.open);
    
    if(file_chooser.run() == Gtk.ResponseType.accept)
    {
        if(this.filename == "")
            this.source_view.source_buf.load_file_data(file_chooser.get_filename(), this);
        else
            var tab = new ide_tab(file_chooser.get_filename());
    }
    
    file_chooser.destroy();
}

function save_file()
{
    if(this.filename == "")
    {
        var file_chooser = new Gtk.FileChooserDialog();
        var file_filter = new Gtk.FileFilter();
        file_filter.add_mime_type("text/javascript");
        file_chooser.set_filter(file_filter);
        file_chooser.add_button("Cancel", Gtk.ResponseType.cancel);
        file_chooser.add_button("Save", Gtk.ResponseType.accept);
        file_chooser.set_action(Gtk.FileChooserAction.save);

        if(file_chooser.run() == Gtk.ResponseType.accept)
        {
            update_filename(file_chooser.get_filename(), this);
        }

        file_chooser.destroy();
    }
    
    if(this.filename != "")
        Gio.simple_write(this.filename, this.source_view.source_buf.text);
}

function undo(button)
{
    this.source_view.source_buf.undo();
    this.source_view.source_buf.update_undo_state(this.toolbar);
}

function redo(button)
{
    this.source_view.source_buf.redo();
    this.source_view.source_buf.update_undo_state(this.toolbar);
}

function execute()
{
    if(this.filename == "")
        return; // TODO: something more intelligent!
    
    Gio.simple_write(this.filename, this.source_view.source_buf.text);
    
    this.terminal.reset(true, true);
    this.terminal.fork_command("/bin/dash");
    
    var command = "clear ; /usr/local/bin/seed \"" + this.filename + "\" ; sleep 1d\n";
    
    this.terminal.feed_child(command, command.length);
}

function ide_toolbar(tab)
{
    actions = new ide_actions(tab);
    this.toolbar = new Gtk.Toolbar();
    
    this.new_button = actions.new_action.create_tool_item();
    this.open_button = actions.open_action.create_tool_item();
    this.save_button = actions.save_action.create_tool_item();
    this.undo_button = actions.undo_action.create_tool_item();
    this.redo_button = actions.redo_action.create_tool_item();
    this.execute_button = actions.execute_action.create_tool_item();
    
    this.toolbar.insert(this.new_button, -1);
    this.toolbar.insert(this.open_button, -1);
    this.toolbar.insert(this.save_button, -1);
    this.toolbar.insert(new Gtk.SeparatorToolItem(), -1);
    this.toolbar.insert(this.undo_button, -1);
    this.toolbar.insert(this.redo_button, -1);
    this.toolbar.insert(new Gtk.SeparatorToolItem(), -1);
    this.toolbar.insert(this.execute_button, -1);
}

