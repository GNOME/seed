#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

var window = new Gtk.Window();
window.signal.hide.connect(Gtk.main_quit);
var toolbar = new Gtk.Toolbar();
var vbox = new Gtk.VBox();
window.add(vbox);

var actions = new Gtk.ActionGroup({name: "toolbar"});
var accels = new Gtk.AccelGroup();

function make_action(def){
    var action = new Gtk.Action(def);
    action.set_accel_group(accels);
    actions.add_action_with_accel(action);
    //Could pass string, but this makes it use default accelerator for STOCK_NEW
    action.connect_accelerator();
    action.signal.activate.connect(
	function (){
	    print(def.tooltip);
	});
    return action;
}

window.add_accel_group(accels);


new_action = make_action({name:"new", label: "New",
			 tooltip:"New File",
			 stock_id:Gtk.STOCK_NEW});
open_action = make_action({name:"open", label: "Open",
			      tooltip:"Open File",
			      stock_id:Gtk.STOCK_OPEN});
save_action = make_action({name:"save", label: "Save",
			   tooltip:"Save File",
			   stock_id:Gtk.STOCK_SAVE});

[new_action, open_action, save_action].forEach(function(action){
    toolbar.insert(action.create_tool_item());
});

var menu = new Gtk.MenuBar();
var file = new Gtk.MenuItem({"child": new Gtk.Label({"label": "File"})});
file_menu = new Gtk.Menu();
file.submenu = file_menu;
menu.append(file);

[new_action, open_action, save_action].forEach(function(action){
    file_menu.append(action.create_menu_item(), -1);
});

vbox.pack_start(menu);
vbox.pack_start(toolbar);

window.resize(300, 300);
window.show_all();

Gtk.main();


