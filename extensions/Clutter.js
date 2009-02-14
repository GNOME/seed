Clutter.Actor.prototype.animate = function(mode, duration, json)
{
	var properties = new Array();
	var endvalues = new Array();
	for (var prop in json)	{
		properties.push(prop);
		endvalues.push(json[prop]);
	}
	return this.animatev(mode, duration, properties, endvalues);

}

Clutter.Actor.prototype.animate_with_timeline = function(mode, timeline, json)
{
	var properties = new Array();
	var endvalues = new Array();
	for (var prop in json)	{
		properties.push(prop);
		endvalues.push(json[prop]);
	}
	return this.animatev_with_timeline(mode, timeline, properties, endvalues);

}
