#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\* type\n\* any\n   \* type\n   \* window\n   \* send_event\n\* expose\n   \* type\n   \* window\n   \* send_event\n   \* area\n      \* x\n      \* y\n      \* width\n      \* height\n      \* intersect\n      \* union\n   \* region\n      \* copy\n      \* destroy\n      \* get_clipbox\n      \* get_rectangles\n      \* empty\n      \* equal\n      \* point_in\n      \* rect_in\n      \* offset\n      \* shrink\n      \* union_with_rect\n      \* intersect\n      \* union\n      \* subtract\n      \* xor\n      \* spans_intersect_foreach\n   \* count\n\* no_expose\n   \* type\n   \* window\n   \* send_event\n\* visibility\n   \* type\n   \* window\n   \* send_event\n   \* state\n\* motion\n   \* type\n   \* window\n   \* send_event\n   \* time\n   \* x\n   \* y\n   \* state\n   \* is_hint\n   \* device\n   \* x_root\n   \* y_root\n\* button\n   \* type\n   \* window\n   \* send_event\n   \* time\n   \* x\n   \* y\n   \* state\n   \* button\n   \* device\n   \* x_root\n   \* y_root\n\* scroll\n   \* type\n   \* window\n   \* send_event\n   \* time\n   \* x\n   \* y\n   \* state\n   \* direction\n   \* device\n   \* x_root\n   \* y_root\n\* key\n   \* type\n   \* window\n   \* send_event\n   \* time\n   \* state\n   \* keyval\n   \* length\n   \* string\n   \* hardware_keycode\n   \* group\n   \* is_modifier\n\* crossing\n   \* type\n   \* window\n   \* send_event\n   \* subwindow\n   \* time\n   \* x\n   \* y\n   \* x_root\n   \* y_root\n   \* mode\n   \* detail\n   \* focus\n   \* state\n\* focus_change\n   \* type\n   \* window\n   \* send_event\n   \* in\n\* configure\n   \* type\n   \* window\n   \* send_event\n   \* x\n   \* y\n   \* width\n   \* height\n\* property\n   \* type\n   \* window\n   \* send_event\n   \* atom\n      \* name\n   \* time\n   \* state\n\* selection\n   \* type\n   \* window\n   \* send_event\n   \* selection\n      \* name\n   \* target\n      \* name\n   \* property\n      \* name\n   \* time\n   \* requestor\n\* owner_change\n   \* type\n   \* window\n   \* send_event\n   \* owner\n   \* reason\n   \* selection\n      \* name\n   \* time\n   \* selection_time\n\* proximity\n   \* type\n   \* window\n   \* send_event\n   \* time\n   \* device\n\* client\n   \* type\n   \* window\n   \* send_event\n   \* message_type\n      \* name\n   \* data_format\n\* dnd\n   \* type\n   \* window\n   \* send_event\n   \* context\n   \* time\n   \* x_root\n   \* y_root\n\* window_state\n   \* type\n   \* window\n   \* send_event\n   \* changed_mask\n   \* new_window_state\n\* setting\n   \* type\n   \* window\n   \* send_event\n   \* action\n   \* name\n\* grab_broken\n   \* type\n   \* window\n   \* send_event\n   \* keyboard\n   \* implicit\n   \* grab_window\n\* put\n\* copy\n\* free\n\* get_time\n\* get_state\n\* get_coords\n\* get_root_coords\n\* get_axis\n\* set_screen\n\* get_screen\n\* send_client_message\n\* send_clientmessage_toall\n\* get_symbol
// STDERR:

Seed.import_namespace("Gdk");

function enum_structlike(indent, e)
{
	for (prop in e)
	{
		Seed.print(indent + "* " + prop);
		if (e[prop] && (e[prop].toString().search("struct") > 0 ||
								e[prop].toString().search("union") > 0))
			enum_structlike(indent + "   ", e[prop]);
	}
}

e = new Gdk.Event();

enum_structlike("", e);
