#include "../../libseed/seed.h"
#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

SeedObject namespace_ref;
SeedClass canvas_class;
SeedEngine * eng;

#define GET_CR cairo_t * cr = seed_object_get_private(this_object)

SeedObject canvas_construct_canvas_from_cairo(SeedContext ctx, cairo_t * cr,
											  SeedException * exception)
{
	SeedObject obj;

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_miter_limit(cr, 10);
	
	obj = seed_make_object(ctx, canvas_class, cr);

	seed_object_set_property(ctx, obj, "globalAlpha",
							 seed_value_from_double(ctx, 1.0, exception));
	seed_object_set_property(ctx, obj, "lineWidth",
							 seed_value_from_double(ctx, 1.0, exception));
	seed_object_set_property(ctx, obj, "miterLimit",
							 seed_value_from_double(ctx, 10, exception));
	seed_object_set_property(ctx, obj, "lineCap",
							 seed_value_from_string(ctx, "butt", exception));
	seed_object_set_property(ctx, obj, "lineJoin",
							 seed_value_from_string(ctx, "miter", exception));
	seed_object_set_property(ctx, obj, "cairo",
							 seed_make_pointer(ctx, cr));
	
	return obj;
}

SeedObject seed_construct_image_canvas(SeedContext ctx,
								   SeedObject constructor,
								   size_t argument_count,
								   const SeedValue arguments[],
								   SeedException * exception)
{
	SeedObject obj;
	cairo_surface_t * surface;
	cairo_t * cr;
	gchar * filename;
	gdouble width, height;
	
	if (argument_count != 3)
	{
		seed_make_exception(ctx, exception, "ArgumentError",
							"Canvas.PDFCanvas constructor expected"
							"3 arguments");
		return (SeedObject)seed_make_null(ctx);
	}
	
	filename = seed_value_to_string(ctx, arguments[0], exception);

	width = seed_value_to_double(ctx, arguments[1], exception);
	height = seed_value_to_double(ctx, arguments[2], exception);
	
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
										 width, height);
	cr = cairo_create(surface);
	cairo_surface_destroy(surface);
	
	obj = canvas_construct_canvas_from_cairo(ctx, cr, exception);

	return obj;
}

SeedObject canvas_construct_pdf_canvas(SeedContext ctx,
								   SeedObject constructor,
								   size_t argument_count,
								   const SeedValue arguments[],
								   SeedException * exception)
{
	SeedObject obj;
	cairo_surface_t * surface;
	cairo_t * cr;
	gchar * filename;
	gdouble width, height;
	
	if (argument_count != 3)
	{
		seed_make_exception(ctx, exception, "ArgumentError",
							"Canvas.PDFCanvas constructor expected"
							"3 arguments");
		return (SeedObject)seed_make_null(ctx);
	}
	
	filename = seed_value_to_string(ctx, arguments[0], exception);

	width = seed_value_to_double(ctx, arguments[1], exception);
	height = seed_value_to_double(ctx, arguments[2], exception);
	
	surface = cairo_pdf_surface_create(filename, width, height);
	cr = cairo_create(surface);
	cairo_surface_destroy(surface);
	
	obj = canvas_construct_canvas_from_cairo(ctx, cr, exception);

	return obj;
}

SeedObject canvas_construct_svg_canvas(SeedContext ctx,
								   SeedObject constructor,
								   size_t argument_count,
								   const SeedValue arguments[],
								   SeedException * exception)
{
	SeedObject obj;
	cairo_surface_t * surface;
	cairo_t * cr;
	gchar * filename;
	gdouble width, height;
	
	if (argument_count != 3)
	{
		seed_make_exception(ctx, exception, "ArgumentError",
							"Canvas.PDFCanvas constructor expected"
							"3 arguments");
		return (SeedObject)seed_make_null(ctx);
	}
	
	filename = seed_value_to_string(ctx, arguments[0], exception);

	width = seed_value_to_double(ctx, arguments[1], exception);
	height = seed_value_to_double(ctx, arguments[2], exception);
	
	surface = cairo_svg_surface_create(filename, width, height);
	cr = cairo_create(surface);
	cairo_surface_destroy(surface);
	
	obj = canvas_construct_canvas_from_cairo(ctx, cr, exception);

	return obj;
}

SeedObject canvas_construct_canvas(SeedContext ctx,
								   SeedObject constructor,
								   size_t argument_count,
								   const SeedValue arguments[],
								   SeedException * exception)
{
	SeedObject obj;
	cairo_t * cr;
	if (argument_count != 1)
	{
		seed_make_exception(ctx, exception, "ArgumentError",
							"Canvas.CairoCanvas constructor"
							"expected 1 argument");
		return (SeedObject)seed_make_null(ctx);
	}

	cr = seed_pointer_get_pointer(ctx, arguments[0]);
	
	obj = canvas_construct_canvas_from_cairo(ctx, cr, exception);
	
	return obj;
}

void seed_canvas_parse_color(cairo_t * cr, gchar * spec, gdouble global_alpha)
{
	if (*spec == '#')
	{
		guint r=0,g=0,b=0,a=0, found;
		// Might be libc dependent (the alpha behaviour);
		if (strlen(spec) > 4)
			found = sscanf(spec, "#%2x%2x%2x%2x", &r, &g, &b, &a);
		else
		{
			found = sscanf(spec, "#%1x%1x%1x%1x", &r, &g, &b, &a);
			r *= 17; g *= 17; b *= 17; a *= 17;
		}
		if (found < 4)
			a = global_alpha*255;

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
			cairo_set_source_rgba(cr, r/255.0, g/255.0, b/255.0, global_alpha);
			return;
		}
		}
	}
	else if (*spec == '[')
	{
		cairo_set_source_rgb(cr, 0, 0, 0);
	}
}

gboolean seed_canvas_set_linewidth (SeedContext ctx,
									SeedObject this_object,
									SeedString property_name,
									SeedValue value,
									SeedException * e)
{
	GET_CR;
	gdouble line_width =
		seed_value_to_double(ctx, value, e);
	
	cairo_set_line_width(cr, line_width);
	return TRUE;
}

gboolean seed_canvas_set_miterlimit (SeedContext ctx,
									SeedObject this_object,
									SeedString property_name,
									SeedValue value,
									SeedException * e)
{
	GET_CR;
	
	cairo_set_miter_limit(cr, seed_value_to_double(ctx, value, e));
}

gboolean seed_canvas_set_linecap (SeedContext ctx,
									SeedObject this_object,
									SeedString property_name,
									SeedValue value,
									SeedException * e)
{
	GET_CR;
	cairo_line_cap_t cap = CAIRO_LINE_CAP_BUTT;
	gchar * line_cap = 
		seed_value_to_string(ctx, value, e);
	
	if (!strcmp(line_cap, "round"))
		cap = CAIRO_LINE_CAP_ROUND;
	else if (!strcmp(line_cap, "square"))
		cap = CAIRO_LINE_CAP_SQUARE;
	
	g_free(line_cap);
	
	cairo_set_line_cap(cr, cap);

}

gboolean seed_canvas_set_linejoin (SeedContext ctx,
									SeedObject this_object,
									SeedString property_name,
									SeedValue value,
									SeedException * e)
{
	GET_CR;
	cairo_line_join_t join = CAIRO_LINE_JOIN_MITER;
	gchar * line_join = 
		seed_value_to_string(ctx, value, e);
	
	if (!strcmp(line_join, "round"))
		join = CAIRO_LINE_JOIN_ROUND;
	else if (!strcmp(line_join, "bevel"))
		join = CAIRO_LINE_JOIN_BEVEL;
	
	g_free(line_join);
	
	cairo_set_line_join (cr, join);
}


void seed_canvas_stroke_setup(SeedContext ctx,
							  SeedObject this_object,
							  cairo_t * cr,
							  SeedException * exception)
{
	gchar * stroke_color = 
		seed_value_to_string(ctx, 
							 seed_object_get_property(ctx, this_object, 
													  "strokeStyle"),
							 exception);
	gdouble global_alpha = 
		seed_value_to_double(ctx, 
							 seed_object_get_property(ctx, this_object, 
													  "globalAlpha"),
							 exception);
	

	seed_canvas_parse_color(cr, stroke_color, global_alpha);

	g_free(stroke_color);
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

SeedValue seed_canvas_stroke_rect  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	GET_CR;
	gdouble x, y, width, height;

	seed_canvas_stroke_setup(ctx, this_object, cr, exception);
	
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
	gdouble global_alpha = 
		seed_value_to_double(ctx, 
							 seed_object_get_property(ctx, this_object, 
													  "globalAlpha"),
							 exception);


	
	seed_canvas_parse_color(cr, stroke_color, global_alpha);
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
	
	seed_canvas_stroke_setup(ctx, this_object, cr, exception);

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
	gdouble global_alpha = 
		seed_value_to_double(ctx, 
							 seed_object_get_property(ctx, this_object, 
													  "globalAlpha"),
							 exception);
	
	seed_canvas_parse_color(cr, stroke_color, global_alpha);
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

SeedValue seed_canvas_rect (SeedContext ctx,
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
	
	cairo_rectangle(cr, x, y, width, height);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_flush (SeedContext ctx,
								 SeedObject function,
								 SeedObject this_object,
								 size_t argument_count,
								 const SeedValue arguments[],
								 SeedException * exception)
{
	GET_CR;
	cairo_surface_t * surface = cairo_get_target(cr);
	
	cairo_surface_flush(surface);
	
	return seed_make_null(ctx);
}

SeedValue seed_canvas_finish (SeedContext ctx,
								 SeedObject function,
								 SeedObject this_object,
								 size_t argument_count,
								 const SeedValue arguments[],
								 SeedException * exception)
{
	GET_CR;
	cairo_surface_t * surface = cairo_get_target(cr);
	
	cairo_surface_finish(surface);

	return seed_make_null(ctx);
}

SeedValue seed_canvas_showpage (SeedContext ctx,
								 SeedObject function,
								 SeedObject this_object,
								 size_t argument_count,
								 const SeedValue arguments[],
								 SeedException * exception)
{
	GET_CR;
	cairo_show_page(cr);
	
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
	{"rect", seed_canvas_rect, 0},
	{"flush", seed_canvas_flush, 0},
	{"finish", seed_canvas_finish, 0},
	{"showPage", seed_canvas_showpage, 0},
	{0, 0, 0}
};

//TODO: Implement getters that use cairo for consistency...
seed_static_value canvas_properties[] = {
	{"lineWidth", 0, seed_canvas_set_linewidth, 0},
	{"lineCap", 0, seed_canvas_set_linecap, 0},
	{"lineJoin", 0, seed_canvas_set_linejoin, 0},
	{"miterLimit", 0, seed_canvas_set_miterlimit, 0},
	{0, 0, 0, 0}
};

void seed_module_init(SeedEngine * local_eng)
{
	SeedObject canvas_constructor, pdf_constructor,
		       svg_constructor, image_constructor;
	seed_class_definition canvas_class_def = seed_empty_class;

	eng = local_eng;

	namespace_ref = seed_make_object(eng->context, 0, 0);

	seed_object_set_property(eng->context, eng->global, "Canvas",
							 namespace_ref);
	
	canvas_class_def.class_name = "CairoCanvas";
	canvas_class_def.static_functions = canvas_funcs;
	canvas_class_def.finalize = canvas_finalize;
	canvas_class_def.static_values = canvas_properties;
	
	canvas_class = seed_create_class(&canvas_class_def);
	
	canvas_constructor = seed_make_constructor(eng->context,
											   canvas_class,
											   canvas_construct_canvas);

	pdf_constructor = seed_make_constructor(eng->context,
											   canvas_class,
											   canvas_construct_pdf_canvas);

	svg_constructor = seed_make_constructor(eng->context,
											   canvas_class,
											   canvas_construct_svg_canvas);
	
	image_constructor = seed_make_constructor(eng->context,
											  canvas_class,
											  seed_construct_image_canvas);
	
	seed_object_set_property(eng->context, namespace_ref, "CairoCanvas",
	canvas_constructor);
	seed_object_set_property(eng->context, namespace_ref, "PDFCanvas",
	pdf_constructor);
	seed_object_set_property(eng->context, namespace_ref, "SVGCanvas",
	svg_constructor);
	seed_object_set_property(eng->context, namespace_ref, "ImageCanvas",
	svg_constructor);
	  
}
