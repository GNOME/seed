FileMenuType = {
	parent: Gtk.Menu.type,
	name: "FileMenu",
	instance_init: function(klass)
	{
		this.newTabItem = new_tab_action.create_menu_item();
		//this.openItem = new Gtk.MenuItem({"child": new Gtk.Label({"label": "Open..."})});
		//this.saveAsItem = new Gtk.MenuItem({"child": new Gtk.Label({"label": "Save As..."})});
		this.closeItem = close_tab_action.create_menu_item();
		this.quitItem = quit_action.create_menu_item();
		this.printItem = print_action.create_menu_item();

		this.append(this.newTabItem);
		//this.append(this.openItem);
		//this.append(this.saveAsItem);
		this.append(this.closeItem);
		this.append(new Gtk.SeparatorMenuItem());
		this.append(this.printItem);	
		this.append(new Gtk.SeparatorMenuItem());
		this.append(this.quitItem);
	}};

FileMenu = new GType(FileMenuType);

EditMenuType = {
	parent: Gtk.Menu.type,
	name: "EditMenu",
	class_init: function(klass, prototype)
	{
	},
	instance_init: function(klass)
	{
		this.cutItem = cut_action.create_menu_item();
		this.copyItem = copy_action.create_menu_item();
		this.pasteItem = paste_action.create_menu_item();
		this.findItem = find_action.create_menu_item();
		
		this.append(this.cutItem);
		this.append(this.copyItem);
		this.append(this.pasteItem);
		this.append(new Gtk.SeparatorMenuItem());
		this.append(this.findItem);
	}};

EditMenu = new GType(EditMenuType);

BookmarksMenuType = {
	parent: Gtk.Menu.type,
	name: "BookmarksMenu",
	class_init: function(klass, prototype)
	{
	},
	instance_init: function(klass)
	{
		this.addBookmarkItem = add_bookmark_action.create_menu_item();
		this.append(this.addBookmarkItem);
		this.append(new Gtk.SeparatorMenuItem());
	}};
BookmarksMenu = new GType(BookmarksMenuType);

HistoryMenuType = {
	parent: Gtk.Menu.type,
	name: "HistoryMenu",
	class_init: function(klass, prototype)
	{
	},
	instance_init: function(klass)
	{
		//TODO: Add something meaningful to the history here
		//this.aboutTabItem = about_action.create_menu_item();
		//this.append(this.aboutTabItem);
	}};

HistoryMenu = new GType(HistoryMenuType);

HelpMenuType = {
	parent: Gtk.Menu.type,
	name: "HelpMenu",
	class_init: function(klass, prototype)
	{
	},
	instance_init: function(klass)
	{
		this.aboutTabItem = about_action.create_menu_item();
		this.append(this.aboutTabItem);
	}};

HelpMenu = new GType(HelpMenuType);


ViewMenuType = {
	parent: Gtk.Menu.type,
	name: "ViewMenu",
	instance_init: function(klass)
	{
		this.zoomInTabItem = zoom_in_action.create_menu_item();
		this.append(this.zoomInTabItem);

		this.zoomOutTabItem = zoom_out_action.create_menu_item();
		this.append(this.zoomOutTabItem);

		this.zoomZeroTabItem = zoom_zero_action.create_menu_item();
		this.append(this.zoomZeroTabItem);
	}};

ViewMenu = new GType(ViewMenuType);


BrowserMenuType = {
	parent: Gtk.MenuBar.type,
	name: "BrowserMenu",
	class_init: function(klass, prototype)
	{
	},
	instance_init: function(klass)
	{
		// Toplevel menus
		this.fileMenu = new FileMenu();
		this.editMenu = new EditMenu();
		this.bookmarksMenu = new BookmarksMenu();
		this.historyMenu = new HistoryMenu();
		this.helpMenu = new HelpMenu();
		this.viewMenu = new ViewMenu();
		
		this.fileItem = new Gtk.MenuItem({"child": new Gtk.Label({"label": "File"})});
		this.editItem = new Gtk.MenuItem({"child": new Gtk.Label({"label": "Edit"})});
		this.viewItem = new Gtk.MenuItem({"child": new Gtk.Label({"label": "View"})});
		this.bookmarksItem = new Gtk.MenuItem({"child": new Gtk.Label({"label": "Bookmarks"})});
		this.historyItem = new Gtk.MenuItem({"child": new Gtk.Label({"label": "History"})});
		this.helpItem = new Gtk.MenuItem({"child": new Gtk.Label({"label": "Help"})});
		
		this.fileItem.submenu = this.fileMenu;
		this.editItem.submenu = this.editMenu;
		this.viewItem.submenu = this.viewMenu;
		this.bookmarksItem.submenu = this.bookmarksMenu;
		this.historyItem.submenu = this.historyMenu;
		this.helpItem.submenu = this.helpMenu;
		
		this.append(this.fileItem);
		this.append(this.editItem);
		this.append(this.viewItem);
		this.append(this.bookmarksItem);
		this.append(this.historyItem);
		this.append(this.helpItem);
	}};

BrowserMenu = new GType(BrowserMenuType);

