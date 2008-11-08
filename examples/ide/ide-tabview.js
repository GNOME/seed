IDETabViewType = {
    parent: Gtk.Notebook,
    name: "IDETabView",
    class_init: function(klass, prototype)
    {
        prototype.create_tab = function (filename)
        {
            var tab = new IDETab();
            
            tab.header = new IDETabHeader();
            tab.header.set_tab(tab);
            tab.source_view.load_file(filename, tab);
            
            this.append_page(tab, tab.header);
            this.set_tab_reorderable(tab, true);
            this.show_all();
            
            this.page = this.get_n_pages() - 1;
            
            this.update_page(this, null, this.page);
        }
        
        prototype.close_tab = function (button)
        {
            tab_view.remove_page(tab_view.page_num(this));
            
            if(tab_view.get_n_pages() == 0)
                tab_view.create_tab("");
        }
        
        prototype.update_page = function (notebook, page, n)
        {
            var my_page = notebook.get_nth_page(n);
            update_window(my_page.source_view.filename);
            toolbar.update_undo_state(my_page.source_view);
        }
    },
    instance_init: function(klass)
    {
        this.signal.switch_page.connect(this.update_page);
    }};

IDETabView = new GType(IDETabViewType);

