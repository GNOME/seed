function new_file()
{
    tab_view.create_tab("");
}

function open_file()
{
    var file_chooser = new Gtk.FileChooserDialog();
    var file_filter = new Gtk.FileFilter();
    file_filter.add_mime_type("text/javascript");
    file_chooser.set_filter(file_filter);
    file_chooser.add_button("Cancel", Gtk.ResponseType.cancel);
    file_chooser.add_button("Open", Gtk.ResponseType.accept);
    file_chooser.set_action(Gtk.FileChooserAction.open);
    
    var current_tab = tab_view.get_nth_page(tab_view.page);
    
    if(file_chooser.run() == Gtk.ResponseType.accept)
    {
        if(current_tab.source_view.filename == "")
            current_tab.source_view.load_file(file_chooser.get_filename(), current_tab);
        else
            tab_view.create_tab(file_chooser.get_filename());
    }
    
    file_chooser.destroy();
}

function save_file()
{
    var current_tab = tab_view.get_nth_page(tab_view.page);
    
    if(current_tab.source_view.filename == "")
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
            current_tab.source_view.update_filename(file_chooser.get_filename(), current_tab);
        }

        file_chooser.destroy();
    }
    
    if(current_tab.source_view.filename != "")
        Gio.simple_write(current_tab.source_view.filename, current_tab.source_view.get_buffer().text);
}

function undo()
{
    var current_tab = tab_view.get_nth_page(tab_view.page);
    current_tab.source_view.get_buffer().undo();
}

function redo()
{
    var current_tab = tab_view.get_nth_page(tab_view.page);
    current_tab.source_view.get_buffer().redo();
}

function execute()
{
}

function init_ide_actions()
{
    actions = new Gtk.ActionGroup({name:"toolbar"});

    accels = new Gtk.AccelGroup();
    window.add_accel_group(accels);

    var new_action = new Gtk.Action({name:"new", label:"New",
                                      tooltip:"New File", stock_id:"gtk-new"});
    new_action.set_accel_group(accels);
    actions.add_action_with_accel(new_action);
    new_action.connect_accelerator();
    new_action.signal.activate.connect(new_file);

    var open_action = new Gtk.Action({name:"open", label:"Open",
                                      tooltip:"Open File", stock_id:"gtk-open"});
    open_action.set_accel_group(accels);
    actions.add_action_with_accel(open_action);
    open_action.connect_accelerator();
    open_action.signal.activate.connect(open_file);

    var save_action = new Gtk.Action({name:"save", label:"Save",
                                      tooltip:"Save File", stock_id:"gtk-save"});
    save_action.set_accel_group(accels);
    actions.add_action_with_accel(save_action);
    save_action.connect_accelerator();
    save_action.signal.activate.connect(save_file);

    var undo_action = new Gtk.Action({name:"undo", label:"Undo",
                                      tooltip:"Undo", stock_id:"gtk-undo"});
    actions.add_action_with_accel(undo_action);
    undo_action.signal.activate.connect(undo);

    var redo_action = new Gtk.Action({name:"redo", label:"Redo",
                                      tooltip:"Redo", stock_id:"gtk-redo"});
    actions.add_action_with_accel(redo_action);
    redo_action.signal.activate.connect(redo);

    var execute_action = new Gtk.Action({name:"execute", label:"Execute",
                                      tooltip:"Execute File", stock_id:"gtk-execute"});
    execute_action.set_accel_group(accels);
    actions.add_action_with_accel(execute_action, "<Control>r");
    execute_action.connect_accelerator();
    execute_action.signal.activate.connect(execute);
}
