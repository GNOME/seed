Gtk = imports.gi.Gtk;

BrowserSettings = imports.BrowserSettings;
TabbedBrowser = imports.TabbedBrowser;

BrowserToolbar = new GType({
    parent: Gtk.HBox.type,
    name: "BrowserToolbar",
    init: function ()
    {
        // Private
        var url_bar = new Gtk.Entry();

        var back_button = new Gtk.ToolButton({stock_id:"gtk-go-back"});
        var forward_button = new Gtk.ToolButton({stock_id:"gtk-go-forward"});
        var refresh_button = new Gtk.ToolButton({stock_id:"gtk-refresh"});

        var back = function ()
        {
            TabbedBrowser.browser.current_tab().get_web_view().go_back();
        };

        var forward = function ()
        {
            TabbedBrowser.browser.current_tab().get_web_view().go_forward();
        };

        var refresh = function ()
        {
            TabbedBrowser.browser.current_tab().get_web_view().reload();
        };

        var browse = function (url)
        {
            TabbedBrowser.browser.current_tab().get_web_view().browse(url.text);
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

        this.set_progress = function (progress)
        {
            if(BrowserSettings.have_progress_bar)
                url_bar.set_progress_fraction(progress);
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
