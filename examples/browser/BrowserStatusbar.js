BrowserStatusbar = new GType({
    parent: Gtk.Statusbar.type,
    name: "BrowserStatusbar",
    init: function (klass)
    {
        // Private
        var progress = new Gtk.ProgressBar();
        var progress_box = new Gtk.VBox();
        var transient_status_group = this.get_context_id("transient");
        
        // Public
        this.get_progress = function ()
        {
            return progress;
        };
        
        this.set_status = function (status)
        {
            this.pop(transient_status_group);
            this.push(transient_status_group, status);
        };
        
        // Implementation
        this.pack_end(progress_box, false, false, 3);
        progress_box.pack_start(progress, true, true, 3);
        progress.set_size_request(-1, 10);
    }
});
