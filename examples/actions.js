#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Gtk.init(null, null);

with(Gtk)
{
	var window = new Window();
	window.signal.hide.connect(function () { main_quit(); });
	var toolbar = new Toolbar();
	var vbox = new VBox();
	window.add(vbox);

	var actions = new ActionGroup({name: "toolbar"});
	var accels = new AccelGroup();

	window.add_accel_group(accels);

	new_action = new Action({name:"new", label: "New",
							 tooltip:"New File",
							 stock_id:STOCK_NEW});
	new_action.set_accel_group(accels);
	actions.add_action_with_accel(new_action);
	//Could pass string, but this makes it use default accelerator for STOCK_NEW
	new_action.connect_accelerator();
	new_action.signal.activate.connect(function () { Seed.print("New file"); });

	open_action = new Action({name:"open", label: "Open",
							  tooltip:"Open File",
							  stock_id:STOCK_OPEN});
	open_action.set_accel_group(accels);
	actions.add_action_with_accel(open_action);
	open_action.connect_accelerator();
	open_action.signal.activate.connect(function () { Seed.print("Open file"); });

	save_action = new Action({name:"save", label: "Save",
							  tooltip:"Save File", 
							  stock_id:STOCK_SAVE});
	save_action.set_accel_group(accels);
	actions.add_action_with_accel(save_action);
	save_action.connect_accelerator();
	save_action.signal.activate.connect(function () { Seed.print("Save file"); });

	toolbar.insert(new_action.create_tool_item());
	toolbar.insert(open_action.create_tool_item());
	toolbar.insert(save_action.create_tool_item());

	var menu = new MenuBar();
	var file = new MenuItem({"child": new Label({"label": "File"})});
	file_menu = new Menu();
	file.submenu = file_menu;
	menu.append(file);

	file_menu.append(new_action.create_menu_item(), -1);
	file_menu.append(open_action.create_menu_item(), -1);
	file_menu.append(save_action.create_menu_item(), -1);

	vbox.pack([{child: menu}, {child:toolbar}]);

	window.resize(300, 300);
	window.show_all();
	
	main();
}

