var tile_svg_size = 50;

function load_svg(file)
{
	var tx = new Clutter.Texture({filename: file});
	tx.filter_quality = Clutter.TextureQuality.HIGH;
	return tx;
}

var colors = [load_svg("blue.svg"), load_svg("green.svg"),
			  load_svg("red.svg"), load_svg("yellow.svg")];

Light = new GType({
	parent: Clutter.Group.type,
	name: "Light",
	init: function(klass)
	{
		// Private
		var closed = false;
		var light_x, light_y;
		var state = Math.floor(Math.random() * max_colors);
		
		// Public
		this.visited = false;
		
		this.on = new Clutter.Clone({source: colors[state],
									 reactive: true});
		
		this.get_state = function ()
		{
			return state;
		};
		
		this.animate_out = function ()
		{
			this.on.anim = this.on.animate(Clutter.AnimationMode.LINEAR,500,
			{
				height: [GObject.TYPE_INT, tile_size * 2],
				width: [GObject.TYPE_INT, tile_size * 2],
				x: [GObject.TYPE_INT, -tile_size/2],
				y: [GObject.TYPE_INT, -tile_size/2]
			});
			this.on.anim.timeline.start();
			
			this.on.anim.timeline.signal.completed.connect(this.hide_light, this);
			
			this.anim = this.animate(Clutter.AnimationMode.LINEAR,500,
			{
				opacity: [GObject.TYPE_UCHAR, 0]
			});
			
			this.anim.timeline.start();
			
			GLib.main_context_iteration();
		};
		
		this.animate_to = function (new_x, new_y, timeline)
		{
			this.anim = this.animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, 500,
			{
				x: [GObject.TYPE_INT, new_x],
				y: [GObject.TYPE_INT, new_y]
			});
			
			this.anim.timeline.start();
			
			GLib.main_context_iteration();
			
			return this.anim.timeline;
		};
		
		this.get_closed = function ()
		{
			return closed;
		};
		
		this.close_tile = function ()
		{
			closed = true;
			this.animate_out();
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

