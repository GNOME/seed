var tile_svg_size = 50;

function load_svg(file)
{
    var tx = new Clutter.Texture({filename: file});
    tx.filter_quality = Clutter.TextureQuality.HIGH;
    return tx;
}

var colors = ["blue.svg", "green.svg", "red.svg", "yellow.svg"];
var loaded_colors = {};

Light = new GType({
    parent: Clutter.Group.type,
    name: "Light",
    init: function()
    {
	// Private
	var closed = false;
	var light_x, light_y;
	var state = Math.floor(Math.random() * max_colors);
		
	// Public
	this.visited = false;
	
	if(loaded_colors[state])
		this.on = new Clutter.Clone({source: loaded_colors[state],
		                             reactive: true});
	else
		this.on = loaded_colors[state] = load_svg(colors[state]);
	
	
	this.get_state = function ()
	{
	    return state;
	};
	
	this.animate_out = function (timeline)
	{
	    this.on.anim = this.on.animate_with_timeline(Clutter.AnimationMode.LINEAR, timeline,
							 {
							     height: tile_size * 2,
							     width: tile_size * 2,
							     x: -tile_size/2,
							     y: -tile_size/2
							 });
	    
	    this.anim = this.animate_with_timeline(Clutter.AnimationMode.LINEAR, timeline,
						   {
						       opacity: 0
						   });
	    
	    timeline.signal.completed.connect(this.hide_light, this);
	    
	    GLib.main_context_iteration();
	};
	
	this.animate_to = function (new_x, new_y, timeline)
	{
	    this.anim = this.animate_with_timeline(Clutter.AnimationMode.EASE_OUT_BOUNCE, timeline,
						   {
						       x: new_x,
						       y: new_y
						   });
	    
	    GLib.main_context_iteration();
	};
	
	this.get_closed = function ()
	{
	    return closed;
	};
	
	this.close_tile = function (timeline)
	{
	    closed = true;
	    this.animate_out(timeline);
	};
	
	this.hide_light = function (timeline, light)
	{
	    light.hide();
	    
	    delete on;
	    
	    if(light.anim)
		delete light.anim;
	    
	    return false;
	};
	
	this.set_light_x = function (new_x)
	{
	    light_x = new_x;
	};
	
	this.set_light_y = function (new_y)
	{
	    light_y = new_y;
	};
	
	this.get_light_x = function ()
	{
	    return light_x;
	};
	
	this.get_light_y = function ()
	{
	    return light_y;
	};
	
	// Implementation
	this.on.set_size(tile_size, tile_size);
	
	this.opacity = 180;
	
	this.set_anchor_point(tile_size / 2, tile_size / 2);
	
	this.add_actor(this.on);
    }
});

