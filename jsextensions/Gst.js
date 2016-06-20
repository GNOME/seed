Gst = imports.gi.Gst;

Gst.Element.prototype.link_many = function(){
    this.link(arguments[0]);
    for (var i = 0; i < arguments.length-1; i++){
	arguments[i].link(arguments[i+1]);
    }
}
