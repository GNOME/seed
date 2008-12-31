#include <gtk/gtk.h>
#include "../../libseed/seed.h"
#include <math.h>
#include <string.h>

#define CANVAS_SIZE 600

SeedEngine		* eng;
SeedClass 		  turtle_class;

GtkWidget 		* instructions;
GtkWidget 		* canvas;
cairo_t 		* offscreen;
cairo_surface_t * offscreen_surface;

typedef struct
{
	// Turtle Properties
	float	direction;
	float	x, y;
	
	// Pen Properties
	float	r, g, b, a;
	int		width;
	int		pen_state;
} turtle_t;

static void destroy(GtkWidget * widget, gpointer data)
{
	gtk_main_quit();
}

static void run_turtle(GtkWidget * widget, gpointer data)
{
	GtkTextBuffer * buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(instructions));
	
	GtkTextIter start, end;	
	gtk_text_buffer_get_start_iter(buf, &start);
	gtk_text_buffer_get_end_iter(buf, &end);
	
	gchar * instruction_str = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
	
	// Clear the offscreen turtle buffer
	cairo_set_source_rgba(offscreen, 1.0, 1.0, 1.0, 1.0);
	cairo_paint(offscreen);
	
	// Create a SeedScript object, which can be evaluated
	// multiple times, using the contents of the GtkTextView
	SeedScript * instruction_script;
	instruction_script = seed_make_script(eng->context, instruction_str, "", 0);
	seed_evaluate(eng->context, instruction_script, NULL);
	
	gtk_widget_queue_draw(canvas);
}

static void draw_turtle(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
	cairo_t * cr = gdk_cairo_create(widget->window);

	// We keep an offscreen buffer, offscreen. Every time the window is exposed,
	// we redraw the offscreen buffer on top of the GtkDrawingArea
	
	cairo_set_source_surface(cr, offscreen_surface, 0, 0);
	cairo_paint(cr);
	
	cairo_destroy(cr);
}

GtkWidget * create_ui()
{
    GtkWidget		* main_pane;        // HPaned
	GtkWidget		* left;				// VBox
	GtkWidget		* button_bar;		// HBox
	GtkWidget		* run;				// Button
	GtkWidget		* canvas_frame;		// Frames
	
	char			* start_script = "var t = new Turtle();\nt.forward(50);"
	   "\nt.turnleft(50);\nt.forward(10);\nt.turnright(50);\nt.forward(60);";
	
	main_pane = gtk_hpaned_new();
	gtk_paned_set_position(GTK_PANED(main_pane), 200);
	
	left = gtk_vbox_new(0, 0);
	instructions = gtk_text_view_new();
	
	GtkTextBuffer * buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(instructions));
	gtk_text_buffer_set_text(buf, start_script, strlen(start_script));
	
	button_bar = gtk_hbox_new(0,0);
	run  = gtk_button_new_with_label("Run");
	gtk_box_pack_start(GTK_BOX(button_bar), run,  TRUE, TRUE, 0);
	
	gtk_box_pack_start(GTK_BOX(left), instructions, TRUE,  TRUE, 3);
	gtk_box_pack_start(GTK_BOX(left), button_bar,   FALSE, TRUE, 3);
	
	canvas = gtk_drawing_area_new();
	gtk_widget_set_size_request(canvas, CANVAS_SIZE, CANVAS_SIZE);
	
	canvas_frame = gtk_frame_new(NULL);
	gtk_container_add(GTK_CONTAINER(canvas_frame), canvas);
	
	gtk_paned_add1(GTK_PANED(main_pane), left);
	gtk_paned_add2(GTK_PANED(main_pane), canvas_frame);
	
	offscreen_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
												   CANVAS_SIZE, CANVAS_SIZE);
	offscreen = cairo_create(offscreen_surface);
	
	g_signal_connect(G_OBJECT(run), "clicked", G_CALLBACK(run_turtle), NULL);
	g_signal_connect(G_OBJECT(canvas), "expose_event", G_CALLBACK(draw_turtle),
					 NULL);
	
	return main_pane;
}

SeedObject turtle_construct(SeedContext ctx,
							SeedObject constructor,
							size_t argument_count,
							const SeedValue arguments[],
							SeedException * exception)
{
	if (argument_count != 0)
	{
		seed_make_exception(ctx, exception, "ArgumentError",
							"Turtle constructor expected 0 arguments.");
		return (SeedObject)seed_make_null(ctx);
	}
	
	turtle_t * t = g_malloc0(sizeof(turtle_t));
	
	t->direction = 0;
	t->x = 50;
	t->y = 50;
	t->pen_state = 1;
	t->r = t->g = t->b = 0;
	t->a = 1;
	t->width = 1;
	
	return seed_make_object(ctx, turtle_class, t);
}

void turtle_draw_line(turtle_t * t, float new_x, float new_y)
{
	cairo_new_path(offscreen);
	cairo_set_source_rgba(offscreen, t->r, t->g, t->b, t->a);
	cairo_set_line_width(offscreen, t->width);
	cairo_move_to(offscreen, t->x, t->y);
	cairo_line_to(offscreen, new_x, new_y);
	
	if(t->pen_state)
		cairo_stroke(offscreen);
	
	t->x = new_x;
	t->y = new_y;
}

SeedValue turtle_forward(SeedContext ctx,
						 SeedObject function,
						 SeedObject this_object,
						 size_t argument_count,
						 const SeedValue arguments[],
						 SeedException * exception)
{
	float new_x, new_y;
	float distance = seed_value_to_float(ctx, arguments[0], NULL);
	
	turtle_t * t = seed_object_get_private(this_object);
	
	new_x = t->x + cosf(t->direction) * distance;
	new_y = t->y + sinf(t->direction) * distance;
	
	turtle_draw_line(t, new_x, new_y);

	return seed_make_null(ctx);
}

SeedValue turtle_turn_left(SeedContext ctx,
						   SeedObject function,
						   SeedObject this_object,
						   size_t argument_count,
						   const SeedValue arguments[],
						   SeedException * exception)
{
	float angle = seed_value_to_float(ctx, arguments[0], NULL);
	
	turtle_t * t = seed_object_get_private(this_object);
	
	t->direction -= (M_PI / 180.0) * angle;

	return seed_make_null(ctx);
}

SeedValue turtle_turn_right(SeedContext ctx,
							SeedObject function,
							SeedObject this_object,
							size_t argument_count,
							const SeedValue arguments[],
							SeedException * exception)
{
	float angle = seed_value_to_float(ctx, arguments[0], NULL);
	
	turtle_t * t = seed_object_get_private(this_object);
	
	t->direction += (M_PI / 180.0) * angle;

	return seed_make_null(ctx);
}

SeedValue turtle_pen_color(SeedContext ctx,
						   SeedObject function,
						   SeedObject this_object,
						   size_t argument_count,
						   const SeedValue arguments[],
						   SeedException * exception)
{
	turtle_t * t = seed_object_get_private(this_object);
	
	t->r = seed_value_to_float(ctx, arguments[0], NULL);
	t->g = seed_value_to_float(ctx, arguments[1], NULL);
	t->b = seed_value_to_float(ctx, arguments[2], NULL);

	if(argument_count == 4)
		t->a = seed_value_to_float(ctx, arguments[3], NULL);

	return seed_make_null(ctx);
}

SeedValue turtle_pen_up(SeedContext ctx,
						SeedObject function,
						SeedObject this_object,
						size_t argument_count,
						const SeedValue arguments[],
						SeedException * exception)
{
	turtle_t * t = seed_object_get_private(this_object);
	
	t->pen_state = FALSE;

	return seed_make_null(ctx);
}

SeedValue turtle_pen_down(SeedContext ctx,
						  SeedObject function,
						  SeedObject this_object,
						  size_t argument_count,
						  const SeedValue arguments[],
						  SeedException * exception)
{
	turtle_t * t = seed_object_get_private(this_object);
	
	t->pen_state = TRUE;

	return seed_make_null(ctx);
}

// Static functions declared on our custom class
seed_static_function turtle_funcs[] = {
	{"forward",		turtle_forward,		0},
	{"turnleft",	turtle_turn_left,	0},
	{"turnright",	turtle_turn_right,	0},
	{"pencolor",	turtle_pen_color,	0},
	{"penup",		turtle_pen_up,		0},
	{"pendown",		turtle_pen_down,	0},
	{0,				0,					0}
};

void turtle_init()
{
	SeedObject turtle_constructor;
	seed_class_definition turtle_class_def = seed_empty_class;
	
	turtle_class_def.class_name = "Turtle";
	turtle_class_def.static_functions = turtle_funcs;

	turtle_class = seed_create_class(&turtle_class_def);
	
	turtle_constructor = seed_make_constructor(eng->context, 
											   turtle_class,
											   turtle_construct);

	// We set our custom class as a property on the global object,
	// so it is available simply as 'Turtle', everywhere.
	seed_object_set_property(eng->context,
							 eng->global, "Turtle", turtle_constructor);
}

int main(int argc, char ** argv)
{
	GtkWidget * window, * ui;

	gtk_init(&argc, &argv);
	
	eng = seed_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	ui = create_ui();
	
	turtle_init();
	
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);
	
	gtk_container_add(GTK_CONTAINER(window), ui);
	gtk_widget_show_all(window);
	
	gtk_main();
	
	return 0;
}
