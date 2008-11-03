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
    this.toolbar = new Gtk.Toolbar();
    
    this.new_button = new Gtk.ToolButton({stock_id:"gtk-new"});
    this.open_button = new Gtk.ToolButton({stock_id:"gtk-open"});
    this.save_button = new Gtk.ToolButton({stock_id:"gtk-save"});
    this.undo_button = new Gtk.ToolButton({stock_id:"gtk-undo"});
    this.redo_button = new Gtk.ToolButton({stock_id:"gtk-redo"});
    this.run_button = new Gtk.ToolButton({stock_id:"gtk-execute"});

    this.new_button.signal_clicked.connect(new_file, tab);
    this.open_button.signal_clicked.connect(open_file, tab);
    this.save_button.signal_clicked.connect(save_file, tab);
    this.undo_button.signal_clicked.connect(undo, tab);
    this.redo_button.signal_clicked.connect(redo, tab);
    this.run_button.signal_clicked.connect(execute, tab);
    
    this.toolbar.insert(this.new_button, -1);
    this.toolbar.insert(this.open_button, -1);
    this.toolbar.insert(this.save_button, -1);
    this.toolbar.insert(new Gtk.SeparatorToolItem(), -1);
    this.toolbar.insert(this.undo_button, -1);
    this.toolbar.insert(this.redo_button, -1);
    this.toolbar.insert(new Gtk.SeparatorToolItem(), -1);
    this.toolbar.insert(this.run_button, -1);
}

