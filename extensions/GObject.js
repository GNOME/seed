GObject = imports.gi.GObject;

(function()
 {
     var types = [{name:"NONE", fundamental: 1},
		  {name:"INTERFACE", fundamental: 2},
		  {name:"CHAR", fundamental: 3},
		  {name:"UCHAR", fundamental: 4},
		  {name:"BOOLEAN", fundamental: 5},
		  {name:"INT", fundamental: 6},
		  {name:"UINT", fundamental: 7},
		  {name:"LONG", fundamental: 8},
		  {name:"ULONG", fundamental: 9},
		  {name:"INT64", fundamental: 10},
		  {name:"UINT64", fundamental: 11},
		  {name:"ENUM", fundamental: 12},
		  {name:"FLAGS", fundamental: 13},
		  {name:"FLOAT", fundamental: 14},
		  {name:"DOUBLE", fundamental: 15},
		  {name:"STRING", fundamental: 16},
		  {name:"POINTER", fundamental: 17},
		  {name:"BOXED", fundamental: 18},
		  {name:"PARAM", fundamental: 19},
		  {name:"OBJECT", fundamental: 20}];
     
     for (var i = 0; i < types.length; i++)
	 {
	     GObject["TYPE_"+types[i].name] = types[i].fundamental << 2;
	 }

 }).apply();
