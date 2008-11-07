function close_tab(button)
{
    if(tabs.get_n_pages() > 1)
    {
        tabs.remove_page(tabs.page_num(this));
    }
    else
    {
        tabs.remove_page(tabs.page_num(this));
        new ide_tab("");
    }
}

function tab_header(nm, tab)
{
    var close_button = new Gtk.Button();
	close_button.set_image(new Gtk.Image({stock: "gtk-close", icon_size: Gtk.IconSize.menu}));
	close_button.signal.clicked.connect(close_tab, tab);
	close_button.set_relief(Gtk.ReliefStyle.none);
	
	this.tab_label = new Gtk.Label({label:nm});
	
	this.tab_header = new Gtk.HBox();
	this.tab_header.pack_start(this.tab_label);
	this.tab_header.pack_start(close_button);
	this.tab_header.show_all();
}

function ide_tab(fn)
{
    this.source_view = new ide_source_view();
    this.terminal = new Vte.Terminal();
    this.toolbar = new ide_toolbar(this);
    this.pane = new Gtk.VPaned();

    this.pane.add1(create_frame(this.source_view.source_view));
    this.pane.add2(create_frame(this.terminal));
    
    this.pane.set_position(600 * (2/3));

    this.source_view.source_buf.signal.changed.connect(text_changed, this.toolbar);
    this.source_view.source_buf.update_undo_state(this.toolbar);
    
    this.main_vbox = new Gtk.VBox();
    this.main_vbox.pack_start(this.toolbar.toolbar);
    this.main_vbox.pack_start(this.pane, true, true);
    
    this.tab_header = new tab_header(this.filename, this.main_vbox);
    
    if(!fn == "")
        this.source_view.source_buf.load_file_data(fn, this);
    else
        update_filename("", this);
    
    tabs.append_page(this.main_vbox, this.tab_header.tab_header);
	tabs.set_tab_reorderable(this.main_vbox, true);
	tabs.show_all();
	
	tabs.page = tabs.get_n_pages() - 1;
}

function change_page(notebook, tab, n)
{
    update_window(tabs.get_tab_label(tabs.get_nth_page(n)).get_children()[0].label);
}

function ide_ui()
{
    tabs = new Gtk.Notebook();
    
    tabs.signal.switch_page.connect(change_page);
    
    tabs.set_scrollable(true);
    
    var tab = new ide_tab("../../ls.js");
}

