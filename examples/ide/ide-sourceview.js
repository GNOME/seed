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
        
        prototype.load = function(new_filename)
        {
            this.update_filename(new_filename, this.tab);
            
            if(this.filename == "")
                return;
            
            try
            {
                var buf = this.get_buffer();
            
                buf.begin_not_undoable_action();
                buf.text = Gio.simple_read(this.filename);
                buf.end_not_undoable_action();
                
                this.update_edited(false);
                
                return 0;
            }
            catch(e)
            {
            	var title = "Could not load the file <i>" + this.filename + "</i>.";
            	this.tab.message_area.show_with_message(title,
            				e.message, Gtk.STOCK_DIALOG_ERROR, this.tab);
            }
            
            return -1;
        }
        
        prototype.save = function()
        {
        	if(this.filename == "")
			{
				var file_chooser = new Gtk.FileChooserDialog();
				var file_filter = new Gtk.FileFilter();
				file_filter.add_mime_type("text/javascript");
				file_chooser.set_filter(file_filter);
				file_chooser.add_button("Cancel", Gtk.ResponseType.cancel);
				file_chooser.add_button("Save", Gtk.ResponseType.accept);
				file_chooser.set_action(Gtk.FileChooserAction.save);

				if(file_chooser.run() == Gtk.ResponseType.accept)
				{
					this.update_filename(file_chooser.get_filename(), current_tab());
					update_window(file_chooser.get_filename());
				}

				file_chooser.destroy();
			}
	
			if(this.filename != "")
			{
				try
				{
					Gio.simple_write(this.filename, this.get_buffer().text);
					this.update_edited(false);
					return 0;
				}
				catch(e)
				{
					var title = "Could not save the file <i>" + this.filename + "</i>.";
            		this.tab.message_area.show_with_message(title,
            				e.message, Gtk.STOCK_DIALOG_ERROR, this.tab);
				}
			}
			
			return -1;
        }
        
        prototype.exception_clear = function ()
        {
            var begin = Gtk.TextIter._new();
            var end = Gtk.TextIter._new();
            
            this.get_buffer().get_start_iter(begin);
            this.get_buffer().get_end_iter(end);
            
            this.get_buffer().remove_source_marks(begin, end, "exception");
        }
        
        prototype.exception_show = function (e)
        {
            var itr = Gtk.TextIter._new();
            this.get_buffer().get_iter_at_line(itr, e.line - 1);
            this.exception_clear();            
            this.get_buffer().create_source_mark("error!!", "exception", itr);
        }
        
        prototype.text_changed = function(sbuf)
        {
            this.update_undo_state();
            this.update_edited(true);
            
            var text = sbuf.text.replace(new RegExp("#!.*"), "");

            try
            {
                Seed.check_syntax(text);
            }
            catch(e)
            {
                this.exception_show(e);
                return;
            }
            
            this.exception_clear();
        }
        
        prototype.update_undo_state = function ()
        {
            actions.get_action("undo").sensitive = this.get_buffer().can_undo;
            actions.get_action("redo").sensitive = this.get_buffer().can_redo;
        }
        
        prototype.update_edited = function (edit)
        {
	        var current_label = this.tab.header.label;
	
	        if(edit)
	        {
		        if(!this.edited)
			        current_label.label = "*" + current_label.label;
	        }
	        else if(this.edited && current_label.label[0] == "*")
		        current_label.label = current_label.label.substr(1);
		    
		    this.edited = edit;
        }
    },
    instance_init: function(klass)
    {
        this.filename = "";
        this.edited = false;
        
        this.set_show_line_numbers(true);
        this.set_show_right_margin(true);
        this.set_highlight_current_line(true);
        this.set_right_margin_position(80);
        this.set_show_line_marks(true);
        this.set_auto_indent(true);
        this.set_indent_width(4);
        
        this.modify_font(Pango.font_description_from_string("monospace 10"));
        
        var epb = new Gtk.Image({"file": "./exception.svg"});
        this.set_mark_category_pixbuf("exception", epb.pixbuf);
        
        var lang_manager = new GtkSource.SourceLanguageManager();
        var js_lang = lang_manager.get_language("js");
        
        var buf = new GtkSource.SourceBuffer({language: js_lang});

        this.set_buffer(buf);
        buf.signal.changed.connect(this.text_changed, this);
        
        var gconf_client = GConf.client_get_default();
        var source_style_mgr = GtkSource.style_scheme_manager_get_default();
        var gedit_style = gconf_client.get_string("/apps/gedit-2/preferences/editor/colors/scheme");
        var source_style = source_style_mgr.get_scheme(gedit_style);
        buf.style_scheme = source_style;
    }};

IDESourceView = new GType(IDESourceViewType);
