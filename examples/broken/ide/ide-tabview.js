IDETabViewType = {
    parent: Gtk.Notebook.type,
    name: "IDETabView",
    class_init: function(klass, prototype)
    {
        prototype.create_tab = function (filename)
        {
            var tab = new IDETab();
            
            tab.header = new IDETabHeader();
            tab.header.close_button.signal.clicked.connect(this.close_tab, tab);
            
            this.append_page(tab, tab.header);
            this.set_tab_reorderable(tab, true);
            
            tab.source_view.load(filename);
            
            this.page = this.get_n_pages() - 1;
        }
        
        prototype.close_tab = function (button)
        {
        	var tab_closing = this;
        	
        	if(button == null)
        		tab_closing = current_tab();
        		
        	var tab_view = tab_closing.parent;
        	
            if(tab_closing.source_view.edited)
            {
            	tab_closing.source_view.query_save();
               	return false;
            }
                     
            tab_view.remove_page(tab_view.page_num(tab_closing));
            
            if(tab_view.get_n_pages() == 0)
                tab_view.create_tab("");
            
            return false;
        }
        
        prototype.force_close_current_tab = function ()
        {
        	var tab_view = current_tab().parent;
        	// TODO: fold this and the above together, somehow
            tab_view.remove_page(tab_view.page_num(current_tab()));
            
            if(tab_view.get_n_pages() == 0)
                tab_view.create_tab("");
        }
        
        prototype.update_page = function (tab_view, page, n)
        {
            var tab = tab_view.get_nth_page(n);
            
        	tab_view.get_toplevel().update_window_title(tab);
           	
            tab.source_view.update_undo_state(tab.source_view);
            
            if(tab.message_area.visible)
            	tab.disable();
            else
            	tab.enable();
        }
        
        prototype.connect_signals = function (tab_view)
        {
        	tab_view.signal.switch_page.connect(tab_view.update_page);
        }
    },
    instance_init: function(klass)
    {
        this.signal.hierarchy_changed.connect(this.connect_signals)
        this.show();
    }};

IDETabView = new GType(IDETabViewType);

