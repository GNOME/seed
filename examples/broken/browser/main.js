#!/usr/bin/env seed


Seed.import_namespace("Gtk");
Seed.import_namespace("WebKit");
Seed.import_namespace("Multiprocessing");
Seed.import_namespace("sqlite");

/*var forker_pipes = new Multiprocessing.Pipe();
var forker_pid = Seed.fork();
if (forker_pid == 0)
{
	var children = new Object;
	print("*** Initializing forker \n");
	print(forker_pipes[0].read());
	forker_pipes[0].write("And the other direction!");
	
	forker_pipes[0].add_watch(1,
			  function(source, condition)
			  {
				  var message = source.read();
				  print("*** forker communication from main: " + message);
				  if (message == "fork\n")
				  {
					  var child_pipes = new Multiprocessing.Pipe();
					  child_pid = Seed.fork();
					  if (child_pid == 0)
					  {
					  }
					  else
					  {
						  source.write(child_pid);
					  }
				  }
				  return true;
			  });
	
	Gtk.main();
}
*/
/* Todo (priority order):
	* Ctrl-click-open-in-new-tab?
	* Bookmark editing/deleting/dropdown on creation allowing you to change the name!!
	* History
	* Search bar
	* Zoom
	* View source (or is web inspector enough? it's nicer than anything I can do)
	* Save / open local files
	* cookies?? do they not happen magically?
	* favicon
	* Settings manager
*/

// Configuration
var homePage = "http://www.google.com";
var selectTabOnCreation = false;
var webKitSettings = new WebKit.WebSettings({enable_developer_extras: true});

Seed.include("browser-bookmarks.js");
Seed.include("browser-actions.js");
Seed.include("browser-toolbar.js");
Seed.include("browser-tab.js");
Seed.include("browser-menu.js");
Seed.include("browser-main.js");
Seed.include("browser-find.js");

Gtk.init(null, null);

function shutdown()
{
	// TODO: make this signaly... or something. wrapper function is silly.
	bookmarks_db.close();
	Gtk.main_quit();
}

/*forker_pipes[1].write("Testing communication from main to forker,"+
					  " received");
print(forker_pipes[1].read());
forker_pipe = forker_pipes[1];*/

var window = new Gtk.Window({title: "Browser"});
window.signal.hide.connect(shutdown);
window.resize(1024,768);
window.add_accel_group(initialize_actions());

//this keeping track of being fullscreen could probably be done better, maybe a BrowserWindow class or something
//like this is kind of dumb, we set this to true since it will be toggled to false
//when the event is generated from the window first appearing, since it's not exclusively
//a fullscreen event.
var window_is_fullscreen = true;
window.signal.window_state_event.connect(function(){window_is_fullscreen = ! window_is_fullscreen; return false});

var status = new Gtk.Statusbar();
// Small race condition with progressBar of sorts.
// ^^ /what/?
var progressBar = new Gtk.ProgressBar();
var menu = new BrowserMenu();
var findBox = new FindBox();

var browserBox = new Gtk.VBox();

populate_bookmarks_menu();

var browser = new TabbedBrowser();
hoverContextId = status.get_context_id("hover");
browser.newTab().navigateTo(homePage);


status.pack_end(progressBar, false, false, 0);

browserBox.pack_start(menu);
browserBox.pack_start(browser, true, true);
browserBox.pack_start(findBox);
browserBox.pack_start(status, false, false, 0);

window.add(browserBox);
window.show_all();
findBox.hide();
Gtk.main();
