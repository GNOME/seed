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
					   x, y,
					   width, height);
	
	return false;
}

IDEMessageAreaType = {
    parent: Gtk.HBox,
    name: "IDEMessageArea",
    class_init: function(klass, prototype)
    {
    	prototype.set_message = function (msg, submsg)
    	{
    		this.text.label = "<b>"+msg+"</b>";
    		this.detail.label = "<small>"+submsg+"</small>";
    	}
    	
    	prototype.set_icon = function (icon)
    	{
    		this.icon.set_from_stock(icon, Gtk.IconSize.dialog);
    	}
    	
    	prototype.show_with_message = function (msg, submsg, icon, tab)
    	{
    		this.tab = tab;
    		
    		this.set_message(msg, submsg);
    		this.set_icon(icon);
    		
    		this.show_message();
    	}
    	
    	prototype.set_buttons = function (buttons)
    	{
    		this.button_hbox.destroy();
			this.button_hbox = new Gtk.HBox({spacing: 6});
			
    		for(but in buttons)
    		{
    			var c_but = Gtk.Button.new_from_stock(buttons[but].stock);
				c_but.signal.clicked.connect(buttons[but].callback, this);
				this.button_hbox.pack_start(c_but, true, false);
    		}
    		
    		this.button_vbox.pack_start(this.button_hbox);
    	}
    	
    	prototype.show_message = function ()
    	{
    		this.tab.disable();
    		this.show_all();
    	}
    	
    	prototype.hide_message = function ()
    	{
    		this.tab.enable();
    		this.hide();
    	}
    },
    instance_init: function(klass)
    {
    	this.signal.expose_event.connect(draw_box, this);
    	
		this.app_paintable = true;
		this.border_width = 8;

		this.inner_box = new Gtk.HBox({spacing:6});
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
		
		this.text_vbox = new Gtk.VBox();
		this.text_vbox.pack_start(this.text, true, true, 4);
		this.text_vbox.pack_start(this.detail, true, true, 4);
		
		this.close = Gtk.Button.new_from_stock(Gtk.STOCK_CLOSE);
		this.close.signal.clicked.connect(this.hide_message, this);
		this.button_hbox = new Gtk.HBox({spacing: 6});
		this.button_hbox.pack_start(this.close);
		
		this.button_vbox = new Gtk.VBox({border_width: 6});
		this.button_vbox.pack_start(this.button_hbox, false, true);
		
		this.inner_box.pack_start(this.icon);
		this.inner_box.pack_start(this.text_vbox, true, true);
		this.inner_box.pack_start(this.button_vbox);
    }};

IDEMessageArea = new GType(IDEMessageAreaType);

