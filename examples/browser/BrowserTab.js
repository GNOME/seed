BrowserTab = new GType({
    parent: Gtk.VBox.type,
    name: "BrowserTab",
    init: function (klass)
    {
        // Private
        var toolbar = new BrowserToolbar();
        var web_view = new BrowserView();
        var scroll_view = new Gtk.ScrolledWindow();
        var tab_label;
        
        // Public
        this.get_toolbar = function ()
        {
            return toolbar;
        };

        this.get_web_view = function ()
        {
            return web_view;
        };
        
        this.set_tab_label = function (new_tab_label)
        {
            tab_label = new_tab_label;
        };
        
        this.get_tab_label = function ()
        {
            return tab_label;
        };
        
        // Implementation
        web_view.set_tab(this);
        
        scroll_view.smooth_scroll = true;
        scroll_view.add(web_view);
        scroll_view.set_policy(Gtk.PolicyType.AUTOMATIC,
                               Gtk.PolicyType.AUTOMATIC);

        this.pack_start(toolbar);
        this.pack_start(scroll_view, true, true);
        this.show_all();
    }
});
