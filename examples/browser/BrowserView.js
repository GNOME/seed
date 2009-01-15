BrowserView = new GType({
    parent: WebKit.WebView.type,
    name: "BrowserView",
    init: function (klass)
    {
        // Private
        var tab;
        
        var install_signals = function (web_view)
        {
            Seed.print("connecting");
            
            web_view.set_scroll_adjustments(null, null);
            web_view.signal.title_changed.connect(update_title);
            web_view.signal.load_committed.connect(update_url);
            web_view.signal.create_web_view.connect(create_new_tab);
            
            Seed.print("connected");
        }
        
        var update_title = function (web_view, web_frame, title)
        {
            if(title.length > 25)
                title = title.slice(0,25) + "...";
            
            tab.get_tab_label().label = title;
        };
        
        var update_url = function (web_view, web_frame)
        {
            var toolbar = tab.get_toolbar();
            
            toolbar.set_url(web_frame.get_uri());
            toolbar.set_can_go_back(web_view.can_go_back());
            toolbar.set_can_go_forward(web_view.can_go_forward());
        };
        
        var create_new_tab = function (web_view, web_frame, new_web_view)
        {
            new_web_view = new WebKit.WebView();
            new_web_view.signal.web_view_ready.connect(show_new_tab);
            return new_web_view;
        }
        
        var show_new_tab = function (new_web_view)
        {
            Seed.print("new tab!");
            //install_signals(new_web_view);
            browser.new_tab("", new_web_view);
        };
        
        // Public
        this.browse = function (url)
        {
            if(url.search("://") < 0)
                url = "http://" + url;

            this.open(url);
        };
        
        this.set_tab = function (new_tab)
        {
            tab = new_tab;
        }
        
        this.get_tab = function ()
        {
            return tab;
        };
        
        // Implementation
        install_signals(this);
    }
});
