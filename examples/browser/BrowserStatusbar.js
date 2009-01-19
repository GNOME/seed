BrowserStatusbar = new GType({
    parent: Gtk.Statusbar.type,
    name: "BrowserStatusbar",
    init: function (klass)
    {
        // Private
        var progress = new Gtk.ProgressBar();
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
        
        // TODO: Determine how to either make smaller progressbar
        //        or, use Gtk SVN's ability to embed it in a text view!
        this.pack_end(progress, false, false, 0);
    }
});
