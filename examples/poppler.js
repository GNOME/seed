#!/usr/local/bin/seed
Seed.import_namespace("cairo");
Seed.import_namespace("Gdk");
Seed.import_namespace("Gtk");
Seed.import_namespace("Poppler");
Seed.import_namespace("PopplerGlib");

Gtk.init(null, null);

var current_document = null;
var drawing_area = null;
var current_page = null;
var page_num = null;
var num_pages = null;

function draw_document()
{
    if (current_page != null)
    {
	drawing_area.window.clear();
        cairo = Gdk.cairo_create(drawing_area.window);
        current_page.render(cairo);
    }
	
    return true;
}
function set_page(num)
{
    current_page = current_document.get_page(num);
    draw_document();
    
    //Get rid of precision.
    entry.text = Seed.sprintf("%d",num+1);
    page_num = num;
    if (page_num == num_pages-1)
	next_button.sensitive = false;
    else
	next_button.sensitive = true;
    if (page_num == 0)
	previous_button.sensitive = false;
    else
	previous_button.sensitive = true;
}

function open_file(sv)
{
    var file_chooser = new Gtk.FileChooserDialog();
    var file_filter = new Gtk.FileFilter();
    file_filter.add_mime_type("application/x-pdf");
    file_chooser.set_filter(file_filter);
    file_chooser.add_button("Cancel", Gtk.ResponseType.cancel);
    file_chooser.add_button("Open", Gtk.ResponseType.accept);
    file_chooser.set_action(Gtk.FileChooserAction.open);
    
    if(file_chooser.run() == Gtk.ResponseType.accept)
    {
	// Poppler.Document will not take a uri as a construction property,
	// use this:
	current_document = 
	    Poppler.Document.new_from_file(file_chooser.get_uri());
	set_page(0);
	num_pages = current_document.get_n_pages();
	page_label.label = " of " + num_pages;
	draw_document();
	if (num_pages > 1)
	    next_button.sensitive = true;
    }
    
    file_chooser.destroy();
}

function make_toolbar()
{
    var window = new Gtk.Window();
    var toolbar = new Gtk.Toolbar();
    var main_vbox = new Gtk.VBox();
    
    var open_button = new Gtk.ToolButton({stock_id:"gtk-open"});
    previous_button = new Gtk.ToolButton({stock_id:"gtk-go-up"});
    next_button = new Gtk.ToolButton({stock_id:"gtk-go-down"});
    var entry_item = new Gtk.ToolItem();
    entry = new Gtk.Entry({text:"0"});
    entry_item.add(entry);
    entry_item.width_request = 40;
    entry.signal_activate.connect(function(){set_page(parseInt(entry.text)-1)});
    var label_item = new Gtk.ToolItem();
    page_label = new Gtk.Label({label: " of 0"});
    label_item.add(page_label);
    
    open_button.signal_clicked.connect(open_file);
    next_button.signal_clicked.connect(
	function(button)
	{
	    set_page(page_num+1);
	});
    previous_button.signal_clicked.connect(
	function(button)
	{
	    set_page(page_num-1);
	});
    next_button.sensitive = previous_button.sensitive = false;
		    
    
    toolbar.insert(open_button,-1);
    toolbar.insert(previous_button, -1);
    toolbar.insert(next_button, -1);
    toolbar.insert(new Gtk.SeparatorToolItem(), -1);
    toolbar.insert(entry_item, -1);
    toolbar.insert(label_item, -1);
    toolbar.insert(new Gtk.SeparatorToolItem(), -1);
	
    return toolbar;
}

var toolbar = make_toolbar();

window = new Gtk.Window();
window.title = "Poppler Demo";
main_vbox = new Gtk.VBox();

drawing_area = new Gtk.DrawingArea();
drawing_area.signal_expose_event.connect(draw_document);

window.add(main_vbox);
main_vbox.pack_start(toolbar);
main_vbox.pack_start(drawing_area, true, true);

window.signal_hide.connect(Gtk.main_quit);

window.show_all();
window.resize(600,800);
Gtk.main();
