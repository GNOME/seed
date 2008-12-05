#include "../../libseed/seed.h"
#include <cairo.h>

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
	if (argument_count != 1)
	{
		seed_make_exception(ctx, exception, "ArgumentError",
							"Canvas.Canvas constructor expected 1 argument");
		return (SeedObject)seed_make_null(ctx);
	}
	return seed_make_object(ctx, canvas_class, 
							seed_pointer_get_pointer(ctx, arguments[0]));
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
	y = seed_value_to_double(ctx, arguments[0], exception);
    width = seed_value_to_double(ctx, arguments[0], exception);
	height = seed_value_to_double(ctx, arguments[0], exception);
		
	cairo_save(cr);
		
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle(cr, x, y, width, height);
	cairo_clip(cr);

	cairo_paint(cr);
	
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
	
	x = seed_value_to_double(ctx, arguments[0], exception);
	y = seed_value_to_double(ctx, arguments[0], exception);
    width = seed_value_to_double(ctx, arguments[0], exception);
	height = seed_value_to_double(ctx, arguments[0], exception);

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
	
	x = seed_value_to_double(ctx, arguments[0], exception);
	y = seed_value_to_double(ctx, arguments[0], exception);
    width = seed_value_to_double(ctx, arguments[0], exception);
	height = seed_value_to_double(ctx, arguments[0], exception);

	cairo_rectangle(cr, x, y, width, height);
	cairo_fill(cr);
	
	return seed_make_null(ctx);
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
	
	canvas_class = seed_create_class(&canvas_class_def);
	
	canvas_constructor = seed_make_constructor(eng->context,
											   canvas_class,
											   canvas_construct_canvas);
	
	seed_object_set_property(eng->context, namespace_ref, "CairoCanvas",
	canvas_constructor);
	  
}
