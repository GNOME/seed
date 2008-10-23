#!/usr/local/bin/seed

Seed.import_namespace("Gdk");
Seed.import_namespace("Gtk");

/* SxS size*/
var size = 5;

Gtk.init(null, null);
var window = new Gtk.Window();
window.title = "Lights Off";
window.signal_hide.connect(Gtk.main_quit);

var wincount = 0;

var table = new Gtk.Table();
table.resize(size,size);

window.add(table);

var buttons = new Array(size);

for (i = 0; i < size; ++i)
{
	buttons[i] = new Array(size);
	for (j = 0; j < size; ++j)
	{
		buttons[i][j] = new Gtk.Button();
		image = new Gtk.Image();
		image.set_from_file("./black.png");
		buttons[i][j].lit = false;
		// everything starts out black
	
		buttons[i][j].x = i;
		buttons[i][j].y = j;

		buttons[i][j].set_image(image);

		buttons[i][j].signal_clicked.connect(buttoon, buttons[i][j]);
		table.attach_defaults(buttons[i][j], j, j+1, i, i+1);
	}
}
/* generate random puzzle */ 
random_clicks();

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

function buttoon( button )
{
	do_click(this.x, this.y);

	if ( wincount == 0 )
	{
		Seed.print("GLORIOUS VICTORY");
		exit(0);
	}
		
}

/* simulate random clicks to generate a random but solvable puzzle */
function random_clicks()
{
	var count = Math.round(20 * Math.random());
	for ( q = 0; q < count; ++q)
	{
		i = Math.round(4 * Math.random());
		j = Math.round(4 * Math.random());
		do_click(i, j);
	}
	//do it again if you won already
	if ( wincount == size * size)
		random_clicks();
}

function flip_color(i, j)
{
	var black = new Gtk.Image();
	black.set_from_file("./black.png");
	var white = new Gtk.Image();
	white.set_from_file("./white.png");
	
	if ( buttons[i][j].lit )
	{
		buttons[i][j].lit = false;
		--wincount;
		buttons[i][j].set_image(black);
	}
	else
	{
		buttons[i][j].lit = true;
		++wincount;
		buttons[i][j].set_image(white);
	}

}

table.show_all();
window.show_all();
Gtk.main();

