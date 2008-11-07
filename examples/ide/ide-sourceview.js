IDESourceViewType = {
    parent: GtkSource.SourceView,
    name: "IDESourceView",
    class_init: function(klass, prototype)
    {
        prototype.update_filename = function(new_filename, tab)
        {
            this.filename = new_filename;
            
            var shortfilename = new_filename.split("/").slice(-1);
            
            if(this.filename != "")
                tab.header.label.label = shortfilename;
            else
                tab.header.label.label = "Untitled";
        }
        
        prototype.load_file = function(new_filename, tab)
        {
            this.update_filename(new_filename, tab);

            // update the window!
            
            if(this.filename == "")
                return;
            
            try
            {
                var buf = this.get_buffer();
            
                buf.begin_not_undoable_action();
                buf.text = Gio.simple_read(this.filename);
                buf.end_not_undoable_action();
            }
            catch(e)
            {
                Seed.print(e.name + " " + e.message);
            }
        }
    },
    instance_init: function(klass)
    {
        this.filename = "";
        
        this.set_show_line_numbers(true);
        this.set_show_right_margin(true);
        this.set_highlight_current_line(true);
        this.set_right_margin_position(80);
        this.set_show_line_marks(true);
        this.set_auto_indent(true);
        this.set_indent_width(4);
        
        this.modify_font(Pango.font_description_from_string("monospace 10"));
        
        //var epb = new Gtk.Image({"file": "./exception.svg"});
        //this.source_view.set_mark_category_pixbuf("exception", epb.pixbuf);
        
        var lang_manager = new GtkSource.SourceLanguageManager();
        var js_lang = lang_manager.get_language("js");
        
        var buf = new GtkSource.SourceBuffer({language: js_lang});

        this.set_buffer(buf);
        
        var gconf_client = GConf.client_get_default();
        var source_style_mgr = GtkSource.style_scheme_manager_get_default();
        var gedit_style = gconf_client.get_string("/apps/gedit-2/preferences/editor/colors/scheme");
        var source_style = source_style_mgr.get_scheme(gedit_style);
        buf.style_scheme = source_style;
    }};

IDESourceView = new GType(IDESourceViewType);
