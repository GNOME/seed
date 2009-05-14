ClutterBox2D = imports.gi.ClutterBox2D;
Clutter = imports.gi.Clutter;
GObject = imports.gi.GObject;

stage = Clutter.Stage.get_default();

const ROWS = (stage.width/64)-1;

function add_static_box(group,x,y,width,height){
	var box = new Clutter.Rectangle();
	box.width = width;
	box.height = height;
	box.x = x; box.y = y;
	group.add_actor (box);
	
	group.child_set_property(box,
		"mode", [GObject.TYPE_LONG, ClutterBox2D.Type.STATIC]);	
}

function add_hand (group,x,y){
	var actor = new Clutter.Texture.from_file("redhand.png");
	group.add_actor (actor);
	actor.opacity = 0xff;
	actor.x = x;
	actor.y = y;

	group.child_set_property(actor,
		"mode", [GObject.TYPE_LONG, ClutterBox2D.Type.DYNAMIC]);	
	group.child_set_property(actor,
	"manipulatable", [GObject.TYPE_BOOLEAN, true]);
}

function add_cage(group){
	var width = stage.width;
	var height = stage.height;

    add_static_box (group, -100, -height*(3-1)-100, width+200, 100);
    add_static_box (group, -100, height, width + 200, 100);

    add_static_box (group, -100, -(height*(5-1)) , 100, height * 5);
    add_static_box (group, width, -(height*(5-1)) , 100, height * 5);	
}

box2d = new ClutterBox2D.ClutterBox2D();


for (var row = 0; row <= ROWS; row++){
	var count = ROWS - row + 1;
	var y = 420 - (row + 1) * 120;
	
	for (var i = 0; i < count; i++){	
		var x = stage.width/2-(count*64/2)+i*64;
		add_hand (box2d, x, y);
	}	
}
add_cage (box2d)

stage.add_actor(box2d);
box2d.simulating = true;

stage.show_all();
