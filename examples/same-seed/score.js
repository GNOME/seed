Score = new GType({
	parent: Clutter.Group.type,
	name: "Score",
	init: function(klass)
	{
		// Private
		var label;
		
		// Public
		this.hide_score = function (timeline, score)
		{
			score.hide();
		};
		
		this.animate_score = function (points)
		{
			if(points <= 0)
				return;
			
			label.set_font_name("Bitstrem Vera Sans Bold 40");
			label.set_text("+" + points);
			this.set_anchor_point(this.width/2, this.height/2);
			
			stage.add_actor(this);
			this.show();
			
			this.anim = this.animate(Clutter.AnimationMode.EASE_OUT_SINE,400,
			{
				depth: [GObject.TYPE_INT, 500],
				opacity: [GObject.TYPE_UCHAR, 0]
			});
			this.anim.timeline.start();
			
			this.anim.timeline.signal.completed.connect(this.hide_score, this);
		};
		
		this.animate_final_score = function (points)
		{
			label.set_font_name("Bitstrem Vera Sans 50");
			label.set_markup("<b>Game Over!</b>\n" + points + " points");
			label.set_alignment(Pango.Alignment.CENTER);
			label.set_color({red:255, green:255, blue:255, alpha:255});
			
			this.set_anchor_point(this.width/2, this.height/2);
			
			stage.add_actor(this);
			this.show();
			
			//this.opacity = 0;
			//this.y = -this.height;
			this.scale_x = this.scale_y = 0;
			
			this.anim = this.animate(Clutter.AnimationMode.EASE_OUT_ELASTIC,2000,
			{
				scale_x: [GObject.TYPE_DOUBLE, 1],
				scale_y: [GObject.TYPE_DOUBLE, 1],
				//y: [GObject.TYPE_INT, stage.width / 2],
				opacity: [GObject.TYPE_UCHAR, 255]
			});
			this.anim.timeline.start();
			
			//this.anim.timeline.signal.completed.connect(this.hide_score, this);
		};
		
		// Implementation
		label = new Clutter.Text();
		label.set_color({red:255, green:255, blue:255, alpha:200});
		
		this.add_actor(label);
		label.show();
		
		this.x = stage.width / 2;
		this.y = stage.height / 2;
	}
});

