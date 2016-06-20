Gtk = imports.gi.Gtk;

(function()
 {
   var pack = function(packing)
   {
     for ( var i in packing )
     {
       var entry = packing[i];
       var expand = entry["expand"];
       var padding = entry["padding"];
       var fill = entry["fill"];
       var child = entry["child"];
       var position = entry["position"];

       if (position == null)
	 position = Gtk.PackType.START;

       this.pack_start(child);
       this.set_child_packing(child,
			      expand,
			      fill,
			      padding,
			      position);
     }
   }
   Gtk.VBox.prototype.pack = pack;
   Gtk.HBox.prototype.pack = pack;
 }).apply();

(function()
 {
   var add_from_string = function(str)
   {
     this.add_from_string_c(str, str.length);
   }
   Gtk.Builder.prototype.add_from_string_c = Gtk.Builder.prototype.add_from_string;
   Gtk.Builder.prototype.add_from_string = add_from_string;
 }).apply();
