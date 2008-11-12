function create_board_size_menu()
{
    var menu = new Gtk.Menu();
    
    var size_5 = new Gtk.MenuItem({"child": new Gtk.Label({"label": "5x5"})});
    var size_7 = new Gtk.MenuItem({"child": new Gtk.Label({"label": "7x7"})});
    var size_9 = new Gtk.MenuItem({"child": new Gtk.Label({"label": "9x9"})});
    
    menu.append(size_5);
    menu.append(size_7);
    menu.append(size_9);

    //size_5.signal.activate.connect(change_size, {size: 5});
    //size_7.signal.activate.connect(change_size, {size: 7});
    //size_9.signal.activate.connect(change_size, {size: 9});
    return menu;
}

function create_menu()
{
    var menu = new Gtk.MenuBar();
    
    var game_menu = new Gtk.Menu();
    var size_item = new Gtk.MenuItem({"child":
                                      new Gtk.Label({"label": "Board Size"})});

    size_item.submenu = create_board_size_menu();
    
    var quit_item = new Gtk.MenuItem({"child":
                                      new Gtk.Label({"label": "Quit"})});
    quit_item.signal.activate.connect(Gtk.main_quit);
    
    game_menu.append(size_item);
    game_menu.append(quit_item);
    
    var game_menu_item = new Gtk.MenuItem({"child":
                                           new Gtk.Label({"label": "Game"})});
    game_menu_item.submenu = game_menu;
    
    menu.append(game_menu_item);
    
    return menu;
}
