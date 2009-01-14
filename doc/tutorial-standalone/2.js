#!/usr/bin/env seed

var home_page = "http://www.google.com";

Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");
Gtk.init(null, null);

Browser = new GType({
    parent: Gtk.VBox.type,
    name: "Browser",
    init: function (klass)
    {
        // Private
        var toolbar = new BrowserToolbar();
        var web_view = new BrowserView();
        var scroll_view = new Gtk.ScrolledWindow();
        
        // Public
        this.get_toolbar = function ()
        {
            return toolbar;
        };

        this.get_web_view = function ()
        {
            return web_view;
        };
        
        // Implementation
        scroll_view.smooth_scroll = true;
        scroll_view.add(web_view);
        scroll_view.set_policy(Gtk.PolicyType.AUTOMATIC,
                               Gtk.PolicyType.AUTOMATIC);

        this.pack_start(toolbar);
        this.pack_start(scroll_view, true, true);
        this.show_all();
    }
});

BrowserView = new GType({
    parent: WebKit.WebView.type,
    name: "BrowserView",
    init: function (klass)
    {
        // Private
        var update_url = function (web_view, web_frame)
        {
            var toolbar = browser.get_toolbar();
            
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
        
        // Implementation
        this.set_scroll_adjustments(null, null);
        this.signal.load_committed.connect(update_url);
    }
});

BrowserToolbar = new GType({
    parent: Gtk.HBox.type,
    name: "BrowserToolbar",
    init: function (klass)
    {
        // Private
        var url_bar = new Gtk.Entry();

        var back_button = new Gtk.ToolButton({stock_id:"gtk-go-back"});
        var forward_button = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
        var refresh_button = new Gtk.ToolButton({stock_id:"gtk-refresh"});

        var back = function ()
        {
            browser.get_web_view().go_back();
        };

        var forward = function ()
        {
            browser.get_web_view().go_forward();
        };

        var refresh = function ()
        {
            browser.get_web_view().reload();
        };

        var browse = function (url)
        {
            browser.get_web_view().browse(url.text);
        };

        // Public
        this.set_url = function (url)
        {
            url_bar.text = url;
        };
        
        this.set_can_go_back = function (can_go_back)
        {
            back_button.sensitive = can_go_back;
        };
        
        this.set_can_go_forward = function (can_go_forward)
        {
            forward_button.sensitive = can_go_forward;
        };
        
        // Implementation
        back_button.signal.clicked.connect(back);
        forward_button.signal.clicked.connect(forward);
        refresh_button.signal.clicked.connect(refresh);
        url_bar.signal.activate.connect(browse);

        this.pack_start(back_button);
        this.pack_start(forward_button);
        this.pack_start(refresh_button);
        this.pack_start(url_bar, true, true);
    }
});

window = new Gtk.Window({title: "Browser"});
window.resize(800, 600);
window.signal.hide.connect(function () { Gtk.main_quit(); });

browser = new Browser();
browser.get_web_view().browse(home_page);
window.add(browser);
window.show_all();

Gtk.main();
