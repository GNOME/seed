Gtk = imports.gi.Gtk;
WebKit = imports.gi.WebKit;

BrowserSettings = imports.BrowserSettings;
BrowserView = imports.BrowserView;
BrowserTab = imports.BrowserTab;

TabbedBrowser = new GType({
    parent: Gtk.Notebook.type,
    name: "TabbedBrowser",
    init: function ()
    {
        // Public
        this.close_tab = function (tab)
        {
            browser.remove_page(browser.page_num(tab));
            tab.destroy();

            if(!browser.get_n_pages())
            {
                browser.new_tab(BrowserSettings.home_page);
            }
        };

        this.new_tab = function (url, new_web_view)
        {
            var new_tab = new BrowserTab.BrowserTab({web_view: new_web_view});

            if(!new_web_view)
                new_tab.get_web_view().browse(url);

            var tab_label = new Gtk.Label({label:"Untitled"});
            var tab_button = new Gtk.Button({relief: Gtk.ReliefStyle.NONE});
            tab_button.set_image(new Gtk.Image({stock: "gtk-close",
                                                icon_size: Gtk.IconSize.MENU}));
            tab_button.signal.clicked.connect(this.close_tab, this);

            var tab_title = new Gtk.HBox();
            tab_title.pack_start(tab_label);
            tab_title.pack_start(tab_button);
            tab_title.show_all();

            new_tab.set_tab_label(tab_label);

            this.append_page(new_tab, tab_title);
            this.set_tab_reorderable(new_tab, true);

            if(BrowserSettings.select_new_tabs)
                this.page = this.page_num(new_tab);
        };

        this.current_tab = function ()
        {
            return this.get_nth_page(this.page);
        };

        // Implementation
        this.new_tab(BrowserSettings.home_page);
    }
});
