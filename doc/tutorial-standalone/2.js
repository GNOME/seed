#!/usr/bin/env seed

var home_page = "http://www.google.com";

Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");
Gtk.init(null, null);

TabbedBrowser = new GType({
    parent: Gtk.Notebook.type,
    name: "TabbedBrowser",
    instance_init: function (klass)
    {
        this.close_tab = function (tab)
        {
            this.remove_page(this.page_num(tab));
            tab.destroy();
            
            if(!browser.get_n_pages())
            {
                this.new_tab(home_page);
            }
        };
        
        this.new_tab = function (url)
        {
            var new_tab = new BrowserTab();
            
            var tab_label = new Gtk.Label({label:"Untitled"});
            var tab_button = new Gtk.Button();
            tab_button.set_image(new Gtk.Image({stock: "gtk-close", 
                                                icon_size: Gtk.IconSize.Menu}));
            tab_button.signal.clicked.connect(this.close_tab, this);
            tab_button.set_relief(Gtk.ReliefStyle.None);
            
            var tab_title = new Gtk.HBox();
            tab_title.pack_start(tab_label);
            tab_title.pack_start(tab_button);
            
            new_tab.set_tab_label(tab_label);
            
            this.append_page(new_tab, tab_title);
        };

        this.current_tab = function ()
        {
            return this.get_nth_page(this.page);
        };

        this.new_tab(home_page);
    }
});

BrowserTab = new GType({
    parent: Gtk.VBox.type,
    name: "BrowserTab",
    instance_init: function (klass)
    {
        var toolbar = new BrowserToolbar();
        var web_view = new BrowserView();
        var scroll_view = new Gtk.ScrolledWindow();
        var tab_label;
        
        web_view.set_tab(this);
        
        scroll_view.smooth_scroll = true;
        scroll_view.add(web_view);
        scroll_view.set_policy(Gtk.PolicyType.Automatic,
                               Gtk.PolicyType.Automatic);

        this.pack_start(toolbar);
        this.pack_start(scroll_view, true, true);
        this.show_all();

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
    }
});

BrowserView = new GType({
    parent: WebKit.WebView.type,
    name: "BrowserView",
    instance_init: function (klass)
    {
        var tab;
        
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
        
        this.set_scroll_adjustments(null, null);
        this.signal.title_changed.connect(update_title);
        this.signal.load_committed.connect(update_url);
    }
});

BrowserToolbar = new GType({
    parent: Gtk.HBox.type,
    name: "BrowserToolbar",
    instance_init: function (klass)
    {
        var url_bar = new Gtk.Entry();

        var back_button = new Gtk.ToolButton({stock_id:"gtk-go-back"});
        var forward_button = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
        var refresh_button = new Gtk.ToolButton({stock_id:"gtk-refresh"});

        var back = function ()
        {
            browser.current_tab().get_web_view().go_back();
        };

        var forward = function ()
        {
            browser.current_tab().get_web_view().go_forward();
        };

        var refresh = function ()
        {
            browser.current_tab().get_web_view().reload();
        };

        var browse = function (url)
        {
            browser.current_tab().get_web_view().browse(url.text);
        };

        back_button.signal.clicked.connect(back);
        forward_button.signal.clicked.connect(forward);
        refresh_button.signal.clicked.connect(refresh);
        url_bar.signal.activate.connect(browse);
        
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

        this.pack_start(back_button);
        this.pack_start(forward_button);
        this.pack_start(refresh_button);
        this.pack_start(url_bar, true, true);
    }
});

window = new Gtk.Window({title: "Browser"});
window.signal.hide.connect(function () { Gtk.main_quit(); });

browser = new TabbedBrowser();
window.add(browser);
window.show_all();

Gtk.main();
