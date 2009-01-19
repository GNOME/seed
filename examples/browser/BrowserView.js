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
        
        var update_progress = function (bar, progress)
        {
            tab.get_statusbar().get_progress().fraction = progress / 100;
        };
        
        var create_new_tab = function (web_view, web_frame, new_web_view)
        {
            new_web_view = new BrowserView();
            new_web_view.signal.web_view_ready.connect(show_new_tab);
            return new_web_view;
        };
        
        var show_new_tab = function (new_web_view)
        {
            browser.new_tab("", new_web_view);
            
            return false;
        };
        
        var hover_link = function (web_view, link, url)
        {
            tab.get_statusbar().set_status(url);
        };
        
        var load_finished = function ()
        {
            tab.get_statusbar().get_progress().hide();
        };
        
        var load_committed = function (web_view, web_frame)
        {
            update_url(web_view, web_frame);
            
            // Show statusbar in committed; otherwise, open-in-new-tab views
            // haven't yet acquired a tab on which to show a progressbar.
            tab.get_statusbar().get_progress().show();
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
        this.signal.load_committed.connect(load_committed);
        this.signal.load_finished.connect(load_finished);
        this.signal.load_progress_changed.connect(update_progress);
        
        this.signal.hovering_over_link.connect(hover_link);
        
        this.signal.create_web_view.connect(create_new_tab);
    }
});
