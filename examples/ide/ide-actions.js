function new_file()
{
    tab_view.create_tab("");
}

function open_file()
{
}

function save_file()
{
}

function undo()
{
}

function redo()
{
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
