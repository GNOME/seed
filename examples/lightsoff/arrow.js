function pushed_arrow()
{
	if(animating_board)
		return true;
		
	// TODO: Need to check that click count is 1
	var direction = (this.flipped ? 1 : -1);
	
	if(score.value + direction < 1)
		return true;
	
	score.set_value(score.value + direction);
	swap_animation(direction);
	gconf_client.set_int("/apps/lightsoff/score", score.value);
	
	return true;
}

ArrowType = {
    parent: Clutter.Group.type,
    name: "Arrow",
    class_init: function(klass, prototype)
    {
    	prototype.set_arrow_flipped = function ()
    	{
    		this.flipped = 1;
    		this.remove_all();
    		
    		var bkg = Clutter.Texture.new_from_file("./arrow-r.svg");
			bkg.filter_quality = Clutter.TextureQuality.high;
			
			this.add_actor(bkg);
    	}
    },
    instance_init: function(klass)
    {
    	this.flipped = 0;
    	var bkg = Clutter.Texture.new_from_file("./arrow-l.svg");
		bkg.filter_quality = Clutter.TextureQuality.high;
		
		this.reactive = true;
		this.signal.button_press_event.connect(pushed_arrow, this);
		
		this.add_actor(bkg);
    }};

Arrow = new GType(ArrowType);

