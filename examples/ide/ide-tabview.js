IDETabViewType = {
    parent: Gtk.Notebook,
    name: "IDETabView",
    class_init: function(klass, prototype)
    {
        prototype.create_tab = function (filename)
        {
            var tab = new IDETab();
            
            tab.header = new IDETabHeader();
            tab.source_view.load_file(filename, tab);
            
            this.append_page(tab, tab.header);
            this.set_tab_reorderable(tab, true);
            this.show_all();
            
            this.page = this.get_n_pages() - 1;
            
            this.update_page(this, null, this.page);
        }
        
        prototype.update_page = function (notebook, page, n)
        {
            update_window(notebook.get_nth_page(n).source_view.filename);
        }
    },
    instance_init: function(klass)
    {
        this.signal.switch_page.connect(this.update_page);
    }};

IDETabView = new GType(IDETabViewType);

