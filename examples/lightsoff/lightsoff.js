#!/usr/local/bin/seed
Seed.import_namespace("Gtk");

image_off = new Gtk.Image({"file": "./tim-off.svg"});
image_on = new Gtk.Image({"file": "./tim-on.svg"});

/* SxS size*/
var size = 5;
var wincount = 0;
var moves = 0;
var timeout = 0;

Gtk.init(null, null);

var window = new Gtk.Window({"title": "Lights Off", "resizable" : false});
var table = new Gtk.Table();
var buttons;
window.signal.hide.connect(Gtk.main_quit);

var vbox = new Gtk.VBox();

vbox.pack_start(create_menu());
vbox.pack_start(table);

window.add(vbox);

create_board();
window.show_all();
initialize_game();

Gtk.main();

function change_size()
{
	destroy_board();
	size = this.size;
	create_board();
	initialize_game();	
	window.show_all();
	return;
}

function destroy_board()
{
	for ( var i = 0; i < size; ++i )
	{
		for ( var j = 0; j < size; ++j )
		{
			buttons[i][j].destroy();
		}
	}
	return;
}

function create_board_size_menu()
{
    var menu = new Gtk.Menu();
    
    var size_5 = new Gtk.MenuItem({"child": new Gtk.Label({"label": "5x5"})});
    var size_7 = new Gtk.MenuItem({"child": new Gtk.Label({"label": "7x7"})});
    var size_9 = new Gtk.MenuItem({"child": new Gtk.Label({"label": "9x9"})});
    
    menu.append(size_5);
    menu.append(size_7);
    menu.append(size_9);

    size_5.signal.activate.connect(change_size, {size: 5});
    size_7.signal.activate.connect(change_size, {size: 7});
    size_9.signal.activate.connect(change_size, {size: 9});
    return menu;
}

function create_board()
{
	table.resize(size,size);
	buttons = new Array(size);

	for (i = 0; i < size; ++i)
	{
		buttons[i] = new Array(size);
		for (j = 0; j < size; ++j)
		{
			buttons[i][j] = new Gtk.Button();
	
			buttons[i][j].x = i;
			buttons[i][j].y = j;

			buttons[i][j].set_relief(Gtk.ReliefStyle.none);
			buttons[i][j].can_focus = false;
		
			buttons[i][j].signal.clicked.connect(button_clicked,
							     buttons[i][j]);
			table.attach_defaults(buttons[i][j], j, j+1, i, i+1);
		}
	}
    
	return;
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

function clear_board()
{
	for(i = 0; i < size; ++i)
	{
		for(j = 0; j < size; ++j)
		{
			buttons[i][j].lit = false;
			buttons[i][j].set_image(new Gtk.Image({"pixbuf": 
							image_off.pixbuf}));
		}
	}
}


function initialize_game()
{
	wincount = 0;
	  
	clear_board();
	Seed.setTimeout("random_clicks()", timeout); // generate random puzzle
	
	moves = 0;
}

function do_click(x , y)
{
	if ( x < size - 1 )
		flip_color(x + 1, y);
	if ( x > 0 )
		flip_color(x - 1, y);
	if ( y < size - 1 )
		flip_color(x, y + 1);
	if ( y > 0 )
		flip_color(x, y - 1);

	flip_color(x,y);
	
	++moves;
}

function button_clicked( button )
{
	do_click(this.x, this.y);

	if ( wincount == 0 )
	{
		Seed.print("GLORIOUS VICTORY in " + moves + " moves!");
		win_animation();
		initialize_game();
	}
}

/* simulate random clicks to generate a random but solvable puzzle */
function random_clicks()
{
	var count = Math.round(size*5* Math.random());

	var sym = Math.floor(3*Math.random());

	for (q = 0; q < count + 5; ++q)
	{
		i = Math.round((size-1) * Math.random());
		j = Math.round((size-1) * Math.random());
	    
		do_click(i, j);
	    
		if (sym == 0)
		{
			do_click(Math.abs(i-(size-1)), j);
		}
		else if (sym == 1)
		{
			do_click(Math.abs(i-(size-1)), Math.abs(j-(size-1)));
		}
		else
		{
			do_click(i,Math.abs(j-(size-1)));
		}
	}
	//do it again if you won already
	if ( wincount == 0 )
		random_clicks();
}

function flip_color(i, j)
{
	if ( buttons[i][j].lit )
	{
		--wincount;
		buttons[i][j].set_image(new Gtk.Image({"pixbuf":
						image_off.pixbuf}));
	}
	else
	{
		++wincount;
		buttons[i][j].set_image(new Gtk.Image({"pixbuf": 
						image_on.pixbuf}));
	}

	buttons[i][j].lit = !buttons[i][j].lit;
}


function lightrow(start, end, col)
{
	if ( end > start )
		for ( i = start; i <= end; ++i )
			Seed.setTimeout("turn_on(buttons["+col+"]["+i+"])", timeout += 30);
	else
		for ( i = start; i >= end; --i )
			Seed.setTimeout("turn_on(buttons["+col+"]["+i+"])", timeout += 30);
}

function turn_on(button)
{
	button.lit = true;
	button.set_image(new Gtk.Image({"pixbuf": image_on.pixbuf}));
}

function lightcol(start, end, row)
{
	if ( end > start )
		for ( var i = start; i <= end; ++i )
			Seed.setTimeout("turn_on(buttons["+i+"]["+row+"])", timeout += 30);
	else
		for ( var i = start; i >= end; --i )
			Seed.setTimeout("turn_on(buttons["+i+"]["+row+"])", timeout += 30);
}

function win_animation()
{
	timeout = 0;
	var max = size - 1;
	var i, j;
	for ( i = 0; i <= max; ++i )
	{
		lightrow(i, max - i, i);
		lightcol(i, max - i, max - i);
		lightrow(max - i, i, max - i); 
		lightcol(max - i, i, i);
	}
}

