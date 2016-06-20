Clutter = imports.gi.Clutter;

var animations = new Array();

Clutter.Actor.prototype.animate = function(mode, duration, json)
{
    var properties = new Array();
    var endvalues = new Array();
    for (var prop in json){
	properties.push(prop);
	endvalues.push(new Array(this.__property_type(prop), json[prop]));
    }
    
    var animation =  this.animatev(mode, duration, properties.length, 
				   properties, endvalues);
    animations.push(animation);
    animation.timeline.signal["completed"].connect(
	function(timeline, obj){
	    animations.splice(animations.indexOf(animation),1);
	}, this);
    
    return animation;
}

Clutter.Actor.prototype.animate_with_timeline = function(mode, timeline, json)
{
    var properties = new Array();
    var endvalues = new Array();
    for (var prop in json)	{
	properties.push(prop);
	endvalues.push(new Array(this.__property_type(prop), json[prop]));
    }

    var animation = 
	this.animate_with_timelinev(mode, timeline, properties.length, 
				    properties, endvalues);
    
    animations.push(animation);
    animation.timeline.signal["completed"].connect(
	function(timeline, obj){
	    animations.splice(animations.indexOf(animation),1);
	}, this);
    
    return animation;
}

Clutter.Actor.prototype.animate_with_alpha = function(alpha, json)
{
    var properties = new Array();
    var endvalues = new Array();
    for (var prop in json)	{
	properties.push(prop);
	endvalues.push(new Array(this.__property_type(prop), json[prop]));
    }

    var animation = 
	this.animate_with_alphav(alpha, properties.length, 
				    properties, endvalues);
    
    animations.push(animation);
    animation.timeline.signal["completed"].connect(
	function(timeline, obj){
	    animations.splice(animations.indexOf(animation),1);
	}, this);
    
    return animation;
}
