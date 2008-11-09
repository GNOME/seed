#!/usr/bin/env seed

Seed.import_namespace("Gtk");

Gtk.init(null, null);

function style_set()
{
	var win = new Gtk.Window({type: Gtk.WindowType.popup});
	win.set_name("gtk-tooltip");
	win.ensure_style();
	this.set_style(win.get_style());
	
	this.queue_draw();
}

function draw_box(widget, event)
{
	var x = Gtk.allocation_get_x(widget.get_allocation());
	var y = Gtk.allocation_get_y(widget.get_allocation());
	var width = Gtk.allocation_get_width(widget.get_allocation());
	var height = Gtk.allocation_get_height(widget.get_allocation());

	Gtk.paint_flat_box(widget.style,
					   widget.window,
					   Gtk.StateType.normal,
					   Gtk.ShadowType.out,
					   null,
					   widget,
					   "tooltip",
					   x + 1, y + 1,
					   width - 2, height - 2);
}

IDEMessageAreaType = {
    parent: Gtk.HBox,
    name: "IDEMessageArea",
    class_init: function(klass, prototype)
    {
    },
    instance_init: function(klass)
    {
    	this.signal.expose_event.connect(draw_box, this);
    	
		this.app_paintable = true;
		this.border_width = 8;

		this.inner_box = new Gtk.HBox();
		this.pack_start(this.inner_box, true, true);
		this.inner_box.signal.style_set.connect(style_set, this);
		
		this.icon = new Gtk.Image({stock: Gtk.STOCK_DIALOG_ERROR,
								   icon_size: Gtk.IconSize.dialog});
		this.text = new Gtk.Label();
		this.detail = new Gtk.Label();
		
		this.text.use_markup = true;
		this.detail.use_markup = true;
		
		this.icon.set_alignment(0.5, 0.5);
		this.text.set_alignment(0, 0.5);
		this.detail.set_alignment(0, 0.5);
		
		this.text_vbox = new Gtk.VBox({border_width: 3});
		this.text_vbox.pack_start(this.text, true, true, 4);
		this.text_vbox.pack_start(this.detail, true, true, 4);
		
		this.close = Gtk.Button.new_from_stock(Gtk.STOCK_CLOSE);
		this.button_vbox = new Gtk.VBox({border_width: 3});
		this.button_vbox.pack_start(this.close, true, false);
		
		this.inner_box.pack_start(this.icon);
		this.inner_box.pack_start(this.text_vbox, true, true);
		this.inner_box.pack_start(this.button_vbox);
    }};

IDEMessageArea = new GType(IDEMessageAreaType);

var window = new Gtk.Window();
window.resize(600, 80);
var ma = new IDEMessageArea();
ma.text.label = "<b>Could not save the file <i>/home/hortont/tim.js</i>.</b>";
ma.detail.label = "<small>You do not have the permissions necessary to save the file.\nPlease check that you typed the location correctly and try again.</small>";
window.add(ma);
window.show_all();

Gtk.main();
