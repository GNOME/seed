TabbedBrowserType = {
	parent: Gtk.Notebook.type,
	name: "TabbedBrowser",
	class_init: function(klass, prototype)
	{
		prototype.newTab = function ()
		{
			var tab = new BrowserTab();
			
			this.append_page(tab, new Gtk.Label({label:"Untitled"}));
			this.set_tab_reorderable(tab, true);
			
			if(selectTabOnCreation)
				this.page = this.get_n_pages() - 1;
			
			return tab;
		}
	},
	instance_init: function(klass)
	{
		this.scrollable = true;
	}};

TabbedBrowser = new GType(TabbedBrowserType);
