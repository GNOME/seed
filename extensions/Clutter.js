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
