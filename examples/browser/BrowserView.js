BrowserView = new GType({
    parent: WebKit.WebView.type,
    name: "BrowserView",
    init: function (klass)
    {
        // Private
        var tab;
        
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
        this.set_scroll_adjustments(null, null);
        this.signal.title_changed.connect(update_title);
        this.signal.load_committed.connect(update_url);
    }
});
