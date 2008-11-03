#!/usr/local/bin/seed

Seed.import_namespace("Vte");
Seed.import_namespace("Gtk");
Seed.import_namespace("GtkSource");
Seed.import_namespace("Gio");

Gtk.init(null, null);

Seed.include("toolbar.js");
Seed.include("tabview.js");

//var itr = Gtk.text_iter_create();

function exception_clear(sbuf)
{
    //var begin = Gtk.text_iter_create();
    //var end = Gtk.text_iter_create();
    
    //sbuf.get_start_iter(begin);
    //sbuf.get_end_iter(end);
    
    //sbuf.remove_source_marks(begin, end, "exception");
}

function exception_line(sbuf, e)
{
    //var itr = Gtk.text_iter_create();

    //sbuf.get_iter_at_line(itr, e.line - 1);
    
    //exception_clear(sbuf);
    
    //sbuf.create_source_mark("error!!", "exception", itr);
}

function text_changed(sbuf)
{
    sbuf.update_undo_state(this);
    
    var text = sbuf.text.replace(/#!.*/, "");
    
    try
    {
        Seed.check_syntax(text);
    }
    catch(e)
    {
        exception_line(sbuf, e);
        return;
    }
    
    exception_clear(sbuf);
}

function update_window(new_filename)
{
    var shortfilename = new_filename.split("/").slice(-1);
    
    if(new_filename != "")
        window.title = "Seed IDE - " + shortfilename;
    else
        window.title = "Seed IDE";
}

function update_filename(new_filename, tab)
{
    tab.filename = new_filename;
    
    update_window(new_filename);
    
    var shortfilename = new_filename.split("/").slice(-1);
    
    if(tab.filename != "")
        tab.tab_header.tab_label.label = shortfilename;
    else
        tab.tab_header.tab_label.label = "Untitled";
    
}

function ide_source_view()
{
    var source_lang_mgr = new GtkSource.SourceLanguageManager();
    var js_lang = source_lang_mgr.get_language("js");

    this.source_buf = new GtkSource.SourceBuffer({language: js_lang});
    
    Seed.prototype(GtkSource.SourceBuffer).load_file_data = function(new_filename, tab)
    {
        update_filename(new_filename, tab);
        
        var file = Gio.file_new_for_path(tab.filename);
        
        this.begin_not_undoable_action();
        this.text = file.read().get_contents();
        this.end_not_undoable_action();
    }
    
    Seed.prototype(GtkSource.SourceBuffer).update_undo_state = function(tbar)
    {
        tbar.undo_button.sensitive = this.can_undo;
        tbar.redo_button.sensitive = this.can_redo;
    }
    
    var epb = new Gtk.Image({"file": "./exception.svg"});
    
    this.source_view = GtkSource.SourceView.new_with_buffer(this.source_buf);
    this.source_view.set_show_line_numbers(true);
    this.source_view.set_show_right_margin(true);
    this.source_view.set_highlight_current_line(true);
    this.source_view.set_right_margin_position(80);
    this.source_view.set_mark_category_pixbuf("exception", epb.pixbuf);
    this.source_view.set_show_line_marks(true);
}

function create_frame(child)
{
    var scroll = new Gtk.ScrolledWindow({vscrollbar_policy: Gtk.PolicyType.automatic, hscrollbar_policy: Gtk.PolicyType.automatic});
    scroll.add(child);
    
    var frame = new Gtk.Frame();
    frame.set_shadow_type(1); // fricking enums don't work.
    frame.add(scroll);
    
    return frame;
}

function ide_init()
{
    window = new Gtk.Window();
    window.resize(600, 600);
    window.signal_hide.connect(Gtk.main_quit);
    ui = new ide_ui();
    window.add(tabs);
    window.show_all();
    
    Gtk.main();
}

ide_init();
