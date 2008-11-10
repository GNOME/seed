function create_scrollbox(child)
{
    var scroll = new Gtk.ScrolledWindow({
                        vscrollbar_policy: Gtk.PolicyType.automatic,
                        hscrollbar_policy: Gtk.PolicyType.automatic});
    scroll.add(child);
    
    return scroll;
}

IDETabType = {
    parent: Gtk.VBox.type,
    name: "IDETab",
    class_init: function(klass, prototype)
    {
    	prototype.disable = function ()
    	{
    		this.source_view.sensitive = false;
    		this.terminal.sensitive = false;
    		actions.get_action("save").sensitive = false;
    		actions.get_action("undo").sensitive = false;
    		actions.get_action("redo").sensitive = false;
    		actions.get_action("execute").sensitive = false;
    		actions.get_action("close").sensitive = false;
    	}
    	
    	prototype.enable = function ()
    	{
    		this.source_view.sensitive = true;
    		this.terminal.sensitive = true;
    		actions.get_action("save").sensitive = true;
    		actions.get_action("execute").sensitive = true;
    		actions.get_action("close").sensitive = true;
    		
    		this.source_view.update_undo_state();
    	}
    },
    instance_init: function(klass)
    {
    	this.message_area = new IDEMessageArea();
		
        this.source_view = new IDESourceView();
        this.source_view.tab = this;
        this.terminal = new Vte.Terminal();
        
        this.vbox = new Gtk.VBox({spacing:3});
        this.vbox.pack_start(this.message_area);
        this.vbox.pack_start(create_scrollbox(this.source_view), true, true);
        
        this.pane = new Gtk.VPaned();
        
        this.pane.add1(this.vbox);
        this.pane.add2(create_scrollbox(this.terminal));
        
        this.pane.set_position(700 * (2/3));
        
        this.pack_start(this.pane, true, true);
        
        this.show_all();
        this.message_area.hide();
    }};

IDETab = new GType(IDETabType);
