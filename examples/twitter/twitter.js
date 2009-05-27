#!/usr/bin/env seed

// Import libraries that are used by the program
Gtk = imports.gi.Gtk;
Gdk = imports.gi.Gdk;
GdkPixbuf = imports.gi.GdkPixbuf;
Gio = imports.gi.Gio;
GLib = imports.gi.GLib;

// Pretty.js is John Resig's date display library. It downloaded it and
// put it in the same directory as this script. You can easily use a lot
// of existing JS libs and import them into Seed applications at runtime.
Pretty = imports.pretty;

// Initialize GTK+
Gtk.init(Seed.argv);

// Create the main application window and set the title
var window = new Gtk.Window({"title": "Twitter Search", "border-width": 5});
// Make the program terminate when the window is closed
window.signal.hide.connect(Gtk.main_quit);

// Modify the default style so that TextView widgets look like labels
Gtk.rc_parse_string(
    'style "tv" {base[NORMAL] = @bg_color} widget_class "*GtkTextView" style "tv"');

function put_pixbuf(container, uri){
    var file = Gio.file_new_for_uri(uri);
    file.read_async(0, null, 
		    function(source, result){
			var loader = new GdkPixbuf.PixbufLoader();
			var stream = source.read_finish(result);
			var dstream = new Gio.DataInputStream.c_new(stream);

			try{
			    while (1){
				loader.write([dstream.read_byte()], 1);
			    }
			}
			catch (e){
			    print(e.name + " " + e.message);
			}
			container.pack_end(
			    new Gtk.Image.from_pixbuf(loader.get_pixbuf()));
			container.show_all();
		    });
}

// This function generates the GTK+ widgets that display the retrieved messages
function make_block(data) {
    var vbox = new Gtk.VBox({"spacing": 10, "border-width": 5});
    var hbox = new Gtk.HBox();

    // The text styling for the heading is done with simple Pango markup.
    var heading = new Gtk.Label({
	"use-markup": true,
	"label": "<b><big>" + data.from_user + "</big></b>\n" +
	    "<small>" + Pretty.prettyDate(data.created_at) + "</small>"
    });

    // The message text is displayed in a TextView widget because the GTK+ label
    // widget completely sucks at wrapping text. The TextView will look like a
    // label because of the RC change at the top of the script.
    var message = new Gtk.TextView({"wrap-mode": 2, "editable": false});
    message.buffer.text = data.text;

    heading.set_alignment(0, 0);
    vbox.pack_start(hbox);
    hbox.pack_start(heading);
    put_pixbuf(hbox, data.profile_image_url);
    vbox.pack_start(message);

    var frame = new Gtk.Frame({"border-width": 5});
    frame.add(vbox);
    frame.show_all();
    return frame;
}

// Create the message container and put it in a scrollable window
var messages = new Gtk.VBox();
var scroll = new Gtk.ScrolledWindow();
scroll.add_with_viewport(messages);
scroll.set_policy(1, 1);

// Create the input textbox and the search button
var textbox = new Gtk.Entry();
var button = new Gtk.Button({"label": "_Search", "use-underline": true});

function async_callback(source, result){
    var stream = source.read_finish(result);
    var dstream = new Gio.DataInputStream.c_new(stream);
    var data = JSON.parse(dstream.read_until("", 0));

    messages.foreach(function(m) {messages.remove(m);});
    data.results.forEach(function(m) {	messages.pack_start(make_block(m));
					while (GLib.main_context_pending())
 					    GLib.main_context_iteration();});

    messages.show_all();
}

// Define the behavior for the button press by associating an anonymous
// function with the button's click signal handler

function do_search(w){
    var twitter =
	Gio.file_new_for_uri("http://search.twitter.com/search.json?q="
			     + textbox.get_text());

    twitter.read_async(0, null, async_callback);
}

button.signal.clicked.connect(do_search);
textbox.signal.activate.connect(do_search);

// Pack the remaining widgets into the window layout
var searchbox = new Gtk.HBox();
searchbox.pack_start(textbox, true, true);
searchbox.pack_start(button);

var layout = new Gtk.VBox({"spacing": 5});
layout.pack_start(searchbox);
layout.pack_start(scroll, true, true);

window.add(layout);
window.show_all();

window.resize(300, 500);
// Start the main GTK+ loop and initiate the program
Gtk.main();

