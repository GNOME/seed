BrowserTab = new GType({
    parent: Gtk.VBox.type,
    name: "BrowserTab",
    class_init: function(klass, prototype)
    {
        // TODO: Robb is promising a prettier interface to this.
        klass.c_install_property(GObject.param_spec_object("web_view",
                                 "WebView",
                                 "WebView to display",
                                 GObject.TYPE_OBJECT,
                                 GObject.ParamFlags.READABLE |
                                     GObject.ParamFlags.WRITABLE));
    },
    init: function (klass)
    {
        // Private
        var toolbar = new BrowserToolbar();
        var local_web_view;
        var scroll_view = new Gtk.ScrolledWindow();
        var tab_label;
        
        // Public
        this.get_toolbar = function ()
        {
            return toolbar;
        };
        
        this.get_web_view = function ()
        {
            return local_web_view;
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
        
        Seed.print("Wv:"); Seed.print(this.web_view);
        
        if(this.web_view == null)
        {
            Seed.print("creating new webview");
            local_web_view = new BrowserView();
        }
        else
        {
            local_web_view = this.web_view;
        }
        
        local_web_view.set_tab(this);
        
        scroll_view.smooth_scroll = true;
        scroll_view.add(local_web_view);
        scroll_view.set_policy(Gtk.PolicyType.AUTOMATIC,
                               Gtk.PolicyType.AUTOMATIC);

        this.pack_start(toolbar);
        this.pack_start(scroll_view, true, true);
        this.show_all();
    }
});
