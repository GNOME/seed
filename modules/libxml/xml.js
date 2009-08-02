xml = imports.libxml;

xml._nodeProto.getElementsByTagName = function(name){
    ret = [];
    child = this.children;
    while (child){
	if (child.name == name)
	    ret.push (child);
	child = child.next;
    }
    return ret;
}

xml._nodeProto.getAttribute = function(name){
    properties = this.properties;
    while (properties){
	if (properties.name == name)
	    return properties.children.content;
	properties = properties.next
    }
    return null;
}
