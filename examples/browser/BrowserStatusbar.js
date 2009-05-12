Gtk = imports.gi.Gtk;
WebKit = imports.gi.WebKit;

BrowserStatusbar = new GType({
    parent: Gtk.Statusbar.type,
    name: "BrowserStatusbar",
    init: function ()
    {
        // Private
        var transient_status_group = this.get_context_id("transient");

        // Public
        this.set_status = function (status)
        {
            this.pop(transient_status_group);
            this.push(transient_status_group, status);
        };

        // Implementation
    }
});
