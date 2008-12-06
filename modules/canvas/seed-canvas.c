#include "../../libseed/seed.h"
#include <cairo.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

SeedObject namespace_ref;
SeedClass canvas_class;
SeedEngine * eng;

#define GET_CR cairo_t * cr = seed_object_get_private(this_object)

SeedObject canvas_construct_canvas(SeedContext ctx,
								   SeedObject constructor,
								   size_t argument_count,
								   const SeedValue arguments[],
								   SeedException * exception)
{
	cairo_t * cr;
	if (argument_count != 1)
	{
		seed_make_exception(ctx, exception, "ArgumentError",
							"Canvas.Canvas constructor expected 1 argument");
		return (SeedObject)seed_make_null(ctx);
	}

	cr = seed_pointer_get_pointer(ctx, arguments[0]);
	
	cairo_set_source_rgba(cr, 0, 0, 0, 1.0);

	return seed_make_object(ctx, canvas_class, 
							cr);
}

SeedValue seed_canvas_save  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	cairo_save(cr);
	
	return seed_make_null(ctx);
}

SeedValue seed_canvas_restore  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	cairo_restore(cr);
	
	return seed_make_null(ctx);
}

SeedValue seed_canvas_scale  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gdouble x = seed_value_to_double(ctx, arguments[0], exception);
	gdouble y = seed_value_to_double(ctx, arguments[1], exception);
	
	cairo_scale(cr, x, y);
	
	return seed_make_null(ctx);
}

SeedValue seed_canvas_translate (SeedContext ctx,
								 SeedObject function,
								 SeedObject this_object,
								 size_t argument_count,
								 const SeedValue arguments[],
								 SeedException * exception)
{
	GET_CR;
	gdouble x = seed_value_to_double(ctx, arguments[0], exception);
	gdouble y = seed_value_to_double(ctx, arguments[1], exception);
	
	cairo_translate(cr, x, y);
	
	return seed_make_null(ctx);
}

SeedValue seed_canvas_rotate  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gdouble x = seed_value_to_double(ctx, arguments[0], exception);
	
	cairo_rotate(cr, x);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_transform  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	cairo_matrix_t matrix;
    gdouble xx, yx, xy, yy, x0, y0;
	
	xx = seed_value_to_double(ctx, arguments[0], exception);
	yx = seed_value_to_double(ctx, arguments[1], exception);
	xy = seed_value_to_double(ctx, arguments[2], exception);
	yy = seed_value_to_double(ctx, arguments[3], exception);
	x0 = seed_value_to_double(ctx, arguments[4], exception);
	y0 = seed_value_to_double(ctx, arguments[5], exception);

	cairo_matrix_init(&matrix, xx, yx, xy, yy, x0, y0);
	cairo_transform(cr, &matrix);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_set_transform  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	cairo_matrix_t matrix;
    gdouble xx, yx, xy, yy, x0, y0;
	
	cairo_identity_matrix(cr);

	xx = seed_value_to_double(ctx, arguments[0], exception);
	yx = seed_value_to_double(ctx, arguments[1], exception);
	xy = seed_value_to_double(ctx, arguments[2], exception);
	yy = seed_value_to_double(ctx, arguments[3], exception);
	x0 = seed_value_to_double(ctx, arguments[4], exception);
	y0 = seed_value_to_double(ctx, arguments[5], exception);
	
	cairo_matrix_init(&matrix, xx, yx, xy, yy, x0, y0);
	cairo_transform(cr, &matrix);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_clear_rect  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gdouble x, y, width, height;
	
	x = seed_value_to_double(ctx, arguments[0], exception);
	y = seed_value_to_double(ctx, arguments[1], exception);
    width = seed_value_to_double(ctx, arguments[2], exception);
	height = seed_value_to_double(ctx, arguments[3], exception);
		
	cairo_save(cr);
		
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, x, y, width, height);
	

	cairo_fill(cr);
	
	cairo_restore(cr);

	return seed_make_null(ctx);
}

void seed_canvas_parse_color(cairo_t * cr, gchar * spec)
{
	if (*spec == '#')
	{
		guint r=0,g=0,b=0,a=0;
		// Might be libc dependent (the alpha behaviour);
		sscanf(spec, "#%2x%2x%2x%2x", &r, &g, &b, &a);
		if (strlen(spec) < 8)
			a = 255;
		cairo_set_source_rgba(cr, r/255.0, g/255.0, b/255.0, a/255.0);
		return;
	}
	else if (*spec == 'r')
	{
		switch(*(spec+3))
		{
		case 'a':
		{
			gint r, g, b;
			gfloat a;
			
			sscanf(spec, "rgba(%d,%d,%d,%f)", &r, &g, &b, &a);
			cairo_set_source_rgba(cr, r/255.0, g/255.0, b/255.0, a);
			return;
		}
		case '(':
		{
			gint r, g, b;

			sscanf(spec, "rgb(%d,%d,%d)", &r, &g, &b);
			cairo_set_source_rgb(cr, r/255.0, g/255.0, b/255.0);
			return;
		}
		}
	}
}

SeedValue seed_canvas_stroke_rect  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gdouble x, y, width, height;
	gchar * stroke_color = 
		seed_value_to_string(ctx, 
							 seed_object_get_property(ctx, this_object, 
													  "strokeStyle"),
							 exception);
	
	seed_canvas_parse_color(cr, stroke_color);
	g_free(stroke_color);
	
	x = seed_value_to_double(ctx, arguments[0], exception);
	y = seed_value_to_double(ctx, arguments[1], exception);
    width = seed_value_to_double(ctx, arguments[2], exception);
	height = seed_value_to_double(ctx, arguments[3], exception);

	cairo_rectangle(cr, x, y, width, height);
	cairo_stroke(cr);
	
	return seed_make_null(ctx);
}
SeedValue seed_canvas_fill_rect  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gdouble x, y, width, height;
	gchar * stroke_color = 
		seed_value_to_string(ctx, 
							 seed_object_get_property(ctx, this_object, 
													  "fillStyle"),
							 exception);
	
	seed_canvas_parse_color(cr, stroke_color);
	g_free(stroke_color);
	
	x = seed_value_to_double(ctx, arguments[0], exception);
	y = seed_value_to_double(ctx, arguments[1], exception);
    width = seed_value_to_double(ctx, arguments[2], exception);
	height = seed_value_to_double(ctx, arguments[3], exception);
	
	cairo_rectangle(cr, x, y, width, height);
	cairo_fill(cr);
	
	return seed_make_null(ctx);
}

SeedValue seed_canvas_end_path  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;

	cairo_close_path(cr);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_begin_path  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	
	cairo_new_path(cr);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_move_to (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gdouble x, y;
	
	x = seed_value_to_double(ctx, arguments[0], exception);
	y = seed_value_to_double(ctx, arguments[1], exception);

	cairo_move_to(cr, x, y);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_line_to  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gdouble x, y;
	
	x = seed_value_to_double(ctx, arguments[0], exception);
	y = seed_value_to_double(ctx, arguments[1], exception);

	cairo_line_to(cr, x, y);
}

SeedValue seed_canvas_stroke (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gchar * stroke_color = 
		seed_value_to_string(ctx, 
							 seed_object_get_property(ctx, this_object, 
													  "strokeStyle"),
							 exception);
	
	seed_canvas_parse_color(cr, stroke_color);
	g_free(stroke_color);
	

	
	cairo_stroke(cr);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_clip  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	
	cairo_clip(cr);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_fill  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gchar * stroke_color = 
		seed_value_to_string(ctx, 
							 seed_object_get_property(ctx, this_object, 
													  "fillStyle"),
							 exception);
	
	seed_canvas_parse_color(cr, stroke_color);
	g_free(stroke_color);
	
	
	cairo_fill(cr);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_arc  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gdouble xc, yc, radius, start, end;
	gboolean counter_clockwise;
	
	xc = seed_value_to_double(ctx, arguments[0], exception);
	yc = seed_value_to_double(ctx, arguments[1], exception);
	radius = seed_value_to_double(ctx, arguments[2], exception);
	start = seed_value_to_double(ctx, arguments[3], exception);
	end = seed_value_to_double(ctx, arguments[4], exception);
	counter_clockwise = seed_value_to_boolean(ctx, arguments[5], exception);
	
	if (counter_clockwise)
		cairo_arc_negative(cr, xc, yc, radius, end, start);
	else
		cairo_arc(cr, xc, yc, radius, start, end);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_quadratic (SeedContext ctx,
								 SeedObject function,
								 SeedObject this_object,
								 size_t argument_count,
								 const SeedValue arguments[],
								 SeedException * exception)
{
	GET_CR;
	
	gdouble p0x, p0y, cp1x, cp1y, cp2x, cp2y, cp3x, cp3y;
	gdouble qp1x, qp1y, qp2x, qp2y;
	
	cairo_get_current_point(cr, &p0x, &p0y);
	
	qp1x = seed_value_to_double(ctx, arguments[0], exception);
	qp1y = seed_value_to_double(ctx, arguments[1], exception);
	qp2x = seed_value_to_double(ctx, arguments[2], exception);
	qp2y = seed_value_to_double(ctx, arguments[3], exception);
	
	cp3x = qp2x;
	cp3y = qp2y;
	
	cp1x = p0x + 2/3.0 * (qp1x-p0x);
	cp1y = p0y + 2/3.0 * (qp1y-p0y);
	
	cp2x = cp1x + 1/3.0 * (qp2x-p0x);
	cp2y = cp1y + 1/3.0 * (qp2y-p0y);
	
	cairo_curve_to(cr, cp1x, cp1y, cp2x, cp2y, cp3x, cp3y);
	
	return seed_make_null(ctx);	
}

SeedValue seed_canvas_bezier (SeedContext ctx,
								 SeedObject function,
								 SeedObject this_object,
								 size_t argument_count,
								 const SeedValue arguments[],
								 SeedException * exception)
{
	GET_CR;
	gdouble cp0x, cp0y, cp1x, cp1y, cp2x, cp2y;
	
	cp0x = seed_value_to_double(ctx, arguments[0], exception);
	cp0y = seed_value_to_double(ctx, arguments[1], exception);
	cp1x = seed_value_to_double(ctx, arguments[2], exception);
	cp1y = seed_value_to_double(ctx, arguments[3], exception);
	cp2x = seed_value_to_double(ctx, arguments[4], exception);
	cp2y = seed_value_to_double(ctx, arguments[5], exception);
	
	cairo_curve_to(cr, cp0x, cp0y, cp1x, cp1y, cp2x, cp2y);

	return seed_make_null(ctx);
}

static void canvas_finalize(SeedObject object)
{
	cairo_destroy((cairo_t *)seed_object_get_private(object));
}

seed_static_function canvas_funcs[] = {
	{"save", seed_canvas_save, 0},
	{"restore", seed_canvas_restore, 0},
	{"scale", seed_canvas_scale, 0},
	{"rotate", seed_canvas_rotate, 0},
	{"translate", seed_canvas_translate, 0},
	{"transform", seed_canvas_transform, 0},
	{"setTransform", seed_canvas_set_transform, 0},
	{"clearRect", seed_canvas_clear_rect, 0},
	{"fillRect", seed_canvas_fill_rect, 0},
	{"strokeRect", seed_canvas_stroke_rect, 0},
	{"beginPath", seed_canvas_begin_path, 0},
	{"closePath", seed_canvas_end_path, 0},
	{"moveTo", seed_canvas_move_to, 0},
	{"lineTo", seed_canvas_line_to, 0},
	{"fill", seed_canvas_fill, 0},
	{"stroke", seed_canvas_stroke, 0},
	{"clip", seed_canvas_clip, 0},
	{"arc", seed_canvas_arc, 0},
	{"quadraticCurveTo", seed_canvas_quadratic, 0},
	{"bezierCurveTo", seed_canvas_bezier, 0},
	{0, 0, 0}
};

void seed_module_init(SeedEngine * local_eng)
{
	SeedObject canvas_constructor;
	seed_class_definition canvas_class_def = seed_empty_class;

	eng = local_eng;

	namespace_ref = seed_make_object(eng->context, 0, 0);

	seed_object_set_property(eng->context, eng->global, "Canvas",
							 namespace_ref);
	
	canvas_class_def.class_name = "CairoCanvas";
	canvas_class_def.static_functions = canvas_funcs;
	canvas_class_def.finalize = canvas_finalize;
	
	canvas_class = seed_create_class(&canvas_class_def);
	
	canvas_constructor = seed_make_constructor(eng->context,
											   canvas_class,
											   canvas_construct_canvas);
	
	seed_object_set_property(eng->context, namespace_ref, "CairoCanvas",
	canvas_constructor);
	  
}
