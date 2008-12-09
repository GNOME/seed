function current_tab()
{
	return browser.get_nth_page(browser.page);
}

function go_back()
{
	current_tab().webView.go_back();
	return false;
}

function go_forward()
{
	current_tab().webView.go_forward();
	return false;
}

function refresh_page()
{
	current_tab().webView.reload();
	return false;
}

function close_tab()
{
	browser.remove_page(browser.page_num(current_tab()));

	if(!browser.get_n_pages())
	{
		browser.newTab().navigateTo(homePage);
	}
	
	return false;
}

function browse(urlBar, tab)
{
	if (urlBar.text.search("://") < 0)
	{
		urlBar.text = "http://" + urlBar.text;
	}
	
	tab.webView.open(urlBar.text);
	
	return false;
}

function initialize_actions()
{
	actions = new Gtk.ActionGroup({name:"toolbar"});

	accels = new Gtk.AccelGroup();

	var new_tab_action = new Gtk.Action({name:"new", label:"New Tab",
		                                 tooltip:"New Tab", stock_id:"gtk-new"});
	new_tab_action.set_accel_group(accels);
	actions.add_action_with_accel(new_tab_action, "<Control>t");
	new_tab_action.connect_accelerator();
	new_tab_action.signal.activate.connect(
		function ()
		{
			browser.newTab().navigateTo(homePage);
			browser.page = browser.get_n_pages() - 1;
		}
	);

	var close_tab_action = new Gtk.Action({name:"close", label:"Close",
		                                   tooltip:"Close Tab", stock_id:"gtk-close"});
	close_tab_action.set_accel_group(accels);
	actions.add_action_with_accel(close_tab_action, "<Control>w");
	close_tab_action.connect_accelerator();
	close_tab_action.signal.activate.connect(close_tab);
	
	var focus_urlbar_action = new Gtk.Action({name:"url", label:"Focus URL Bar",
		                        tooltip:"Focus URL Bar"});
	focus_urlbar_action.set_accel_group(accels);
	actions.add_action_with_accel(focus_urlbar_action, "<Control>l");
	focus_urlbar_action.connect_accelerator();
	focus_urlbar_action.signal.activate.connect(
		function ()
		{
			current_tab().toolbar.urlBar.grab_focus();
		}
	);
	
	return accels;
}
