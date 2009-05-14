Clutter = imports.gi.Clutter;

Clutter.Actor.prototype.animate = function(mode, duration, json)
{
	var properties = new Array();
	var endvalues = new Array();
	for (var prop in json)	{
	    properties.push(prop);
	    endvalues.push([this.__property_type(prop), json[prop]]);
	}
	return this.animatev(mode, duration, properties.length, 
			     properties, endvalues);

}

Clutter.Actor.prototype.animate_with_timeline = function(mode, timeline, json)
{
	var properties = new Array();
	var endvalues = new Array();
	for (var prop in json)	{
		properties.push(prop);
		endvalues.push(json[prop]);
	}
	return this.animate_with_timelinev(mode, timeline, properties.length, 
			     properties, endvalues);

}
