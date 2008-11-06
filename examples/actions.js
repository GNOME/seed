#!/usr/local/bin/seed
Seed.import_namespace("Gtk");
Gtk.init(null, null);

window = new Gtk.Window();
toolbar = new Gtk.Toolbar();
vbox = new Gtk.VBox();
window.add(vbox);

actions = new Gtk.ActionGroup({name: "toolbar"});
accels = new Gtk.AccelGroup();

window.add_accel_group(accels);

new_action = new Gtk.Action({name:"new", label: "New",
			      tooltip:"New File", stock_id:Gtk.STOCK_NEW});
new_action.set_accel_group(accels);
actions.add_action_with_accel(new_action);
//Could pass string, but this makes it use default accelerator for gtk-new
new_action.connect_accelerator();
new_action.signal_activate.connect(function(){Seed.print("New file")});

open_action = new Gtk.Action({name:"open", label: "Open",
			      tooltip:"Open File", stock_id:Gtk.STOCK_OPEN});
open_action.set_accel_group(accels);
actions.add_action_with_accel(open_action);
open_action.connect_accelerator();
open_action.signal_activate.connect(function(){Seed.print("Open file")});

save_action = new Gtk.Action({name:"save", label: "Save",
			      tooltip:"Save File", stock_id:Gtk.STOCK_SAVE});
save_action.set_accel_group(accels);
actions.add_action_with_accel(save_action);
save_action.connect_accelerator();
save_action.signal_activate.connect(function(){Seed.print("Save file")});


toolbar.insert(new_action.create_tool_item());
toolbar.insert(open_action.create_tool_item());
toolbar.insert(save_action.create_tool_item());

menu = new Gtk.MenuBar();
file = new Gtk.MenuItem({"child":
			 new Gtk.Label({"label": "File"})});
file_menu = new Gtk.Menu();
file.submenu = file_menu;
menu.append(file);

file_menu.append(new_action.create_menu_item(), -1);
file_menu.append(open_action.create_menu_item(), -1);
file_menu.append(save_action.create_menu_item(), -1);

packing = [{child: menu}, {child:toolbar}];
vbox.pack(packing);

window.show_all();
window.width_request = 150;
Gtk.main();


