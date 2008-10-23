#!/usr/local/bin/seed
Seed.import_namespace("Gtk");

image_off = new Gtk.Image({"file": "./tim-off.svg"});
image_on = new Gtk.Image({"file": "./tim-on.svg"});

/* SxS size*/
var size = 20;
var wincount = 0;

function create_board()
{
    var table = new Gtk.Table();
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
		
		    buttons[i][j].signal_clicked.connect(button_clicked, buttons[i][j]);
		    table.attach_defaults(buttons[i][j], j, j+1, i, i+1);
	    }
    }
    
    return table;
}

function clear_board()
{
    for(i = 0; i < size; ++i)
    {
        for(j = 0; j < size; ++j)
        {
            buttons[i][j].lit = false;
            buttons[i][j].set_image(new Gtk.Image({"pixbuf": image_off.pixbuf}));
        }
    }
}

function initialize_game()
{
    wincount = 0;
    
    clear_board();
    random_clicks(); // generate random puzzle
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
}

function button_clicked( button )
{
	do_click(this.x, this.y);

	if ( wincount == 0 )
	{
		Seed.print("GLORIOUS VICTORY");
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
		buttons[i][j].set_image(new Gtk.Image({"pixbuf": image_off.pixbuf}));
	}
	else
	{
		++wincount;
		buttons[i][j].set_image(new Gtk.Image({"pixbuf": image_on.pixbuf}));
	}

    buttons[i][j].lit = !buttons[i][j].lit;
}

Gtk.init(null, null);

var window = new Gtk.Window({"title": "Lights Off", "resizable" : false});
window.signal_hide.connect(Gtk.main_quit);
window.add(create_board());
window.show_all();
initialize_game();

Gtk.main();

