Seed.import_namespace("GLib");

var bookmark_path = GLib.get_home_dir() + "/.seed_browser_bookmarks.db";

//check if the bookmarks db exists and if there are permissions

var new_db = (!GLib.file_test(bookmark_path, GLib.FileTest.EXISTS));

bookmarks_db = new sqlite.Database(bookmark_path);

if(new_db)
	bookmarks_db.exec("create table bookmarks (key INTEGER PRIMARY KEY, url TEXT, name TEXT);");

function populate_bookmarks_menu()
{
	bookmarks_db.exec("select * from bookmarks",
		function(results)
	    {
			var new_bookmark_item = Gtk.MenuItem.new_with_label(results["name"]);
			new_bookmark_item.tooltip_text = results["url"];
			new_bookmark_item.signal.activate.connect(select_bookmark, null, results);
			menu.bookmarksMenu.append(new_bookmark_item);
		});

}

function add_bookmark()
{
	// This right here is the second largest security vulnerability I've ever knowingly written.
	// TODO: ROBB, we need a substitution api...
	// Also it's just stupid, because it'll break on quotes, etc. And we all know sanitizing your SQL yourself is retarded.

	var b_url = current_tab().webView.get_main_frame().get_uri();
	//var b_name = current_tab().webView.get_main_frame().get_title();
	var b_name = current_tab().titleLabel.label;
	//var b_name = b_url; // ^ something about in arguments makes this not work FIXIT. the gir says no args.
	bookmarks_db.exec("insert into bookmarks (url, name) values ('" + b_url + "','" + b_name + "');");
	bookmarks_db.exec("select * from bookmarks where url='" + b_url + "' AND name='" + b_name + "';",
					  function(results){g_results=results}); // TODO: there's a better way to do this.
	// RACERACERACE
	// probably need to save IDs-to-menuitems so we can destroy them if we want, etc...
	// todo: how do we get the ID out when we insert??
	var new_bookmark_item = Gtk.MenuItem.new_with_label(b_name);
	new_bookmark_item.tooltip_text = b_url;
	new_bookmark_item.signal.activate.connect(select_bookmark, g_results);
	new_bookmark_item.show_all();

	menu.bookmarksMenu.append(new_bookmark_item);
}

function select_bookmark(menuItem, results)
{
	current_tab().navigateTo(results["url"]);
}
