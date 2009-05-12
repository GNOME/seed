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
