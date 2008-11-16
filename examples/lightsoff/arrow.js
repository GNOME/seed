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
		
		this.add_actor(bkg);
    }};

Arrow = new GType(ArrowType);

