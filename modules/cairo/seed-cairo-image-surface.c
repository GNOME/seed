#include <seed.h>
#include <cairo/cairo.h>

#include "seed-cairo.h"
#include "seed-cairo-surface.h"

#define CAIRO_SURFACE_PRIV(obj) ((cairo_surface_t *)seed_object_get_private(obj))

#define CHECK_SURFACE(obj, res) ({						\
    if (!seed_object_is_of_class (ctx, obj, seed_cairo_image_surface_class)){	\
      seed_make_exception (ctx, exception, "ArgumentError", "Object is not a Cairo Surface"); \
      return seed_make_##res (ctx);\
    }									\
    if (!seed_object_get_private (obj)){				\
      seed_make_exception (ctx, exception, "ArgumentError", "Cairo surface has been destroyed"); \
      return seed_make_##res (ctx);}})

#define CHECK_THIS(res) if (!seed_object_get_private (this_object)){	\
    seed_make_exception (ctx, exception, "ArgumentError", "Cairo surface has been destroyed"); \
      return seed_make_##res (ctx);}

SeedClass seed_cairo_image_surface_class;

static SeedObject
seed_object_from_cairo_image_surface (SeedContext ctx, cairo_surface_t *surf)
{
  SeedObject jsobj;
  
  jsobj = cairo_surface_get_user_data (surf, seed_get_cairo_key());
  if (jsobj)
    return jsobj;
  
  jsobj = seed_make_object (ctx, seed_cairo_image_surface_class, surf);
  cairo_surface_set_user_data (surf, seed_get_cairo_key(), jsobj, seed_cairo_destroy_func);
  return jsobj;
}

static SeedValue
seed_cairo_image_surface_get_format (SeedContext ctx,
				SeedObject function,
				SeedObject this_object,
				gsize argument_count,
				const SeedValue arguments[],
				SeedException *exception)
{
  CHECK_THIS(null);
  return seed_value_from_long (ctx, cairo_image_surface_get_format (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedValue
seed_cairo_image_surface_get_width (SeedContext ctx,
				SeedObject function,
				SeedObject this_object,
				gsize argument_count,
				const SeedValue arguments[],
				SeedException *exception)
{
  CHECK_THIS(null);
  return seed_value_from_int (ctx, cairo_image_surface_get_width (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedValue
seed_cairo_image_surface_get_height (SeedContext ctx,
				SeedObject function,
				SeedObject this_object,
				gsize argument_count,
				const SeedValue arguments[],
				SeedException *exception)
{
  CHECK_THIS(null);
  return seed_value_from_int (ctx, cairo_image_surface_get_height (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedValue
seed_cairo_image_surface_get_stride (SeedContext ctx,
				SeedObject function,
				SeedObject this_object,
				gsize argument_count,
				const SeedValue arguments[],
				SeedException *exception)
{
  CHECK_THIS(null);
  return seed_value_from_int (ctx, cairo_image_surface_get_stride (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedObject
seed_cairo_construct_image_surface (SeedContext ctx,
				    SeedObject constructor,
				    size_t argument_count,
				    const SeedValue arguments[],
				    SeedException * exception)
{
  cairo_surface_t *ret;
  gint width, height;
  cairo_format_t format;
  if (argument_count != 3)
    {
      EXPECTED_EXCEPTION("ImageSurface", "3 arguments");
    }
  
  format = seed_value_to_long (ctx, arguments[0], exception);
  width = seed_value_to_int (ctx, arguments[1], exception);
  height = seed_value_to_int (ctx, arguments[2], exception);
  ret = cairo_image_surface_create (format, width, height);
  
  return seed_object_from_cairo_image_surface (ctx, ret);
}

seed_static_function image_surface_funcs[] = {
  //  {"get_data", seed_cairo_image_surface_get_data, 0},
  {"get_format", seed_cairo_image_surface_get_format, 0},
  {"get_width", seed_cairo_image_surface_get_width, 0},
  {"get_height", seed_cairo_image_surface_get_height, 0},
  {"get_stride", seed_cairo_image_surface_get_stride, 0},
  {0, 0, 0}
};

void
seed_define_cairo_image_surface (SeedContext ctx,
				 SeedObject namespace_ref)
{
  SeedObject constructor_ref;
  seed_class_definition image_def = seed_empty_class;
  
  image_def.class_name = "CairoImageSurface";
  image_def.static_functions = image_surface_funcs;
  image_def.parent_class = seed_get_cairo_surface_class ();
  seed_cairo_image_surface_class = seed_create_class (&image_def);
  
  constructor_ref = seed_make_constructor (ctx,
					   seed_cairo_image_surface_class,
					   seed_cairo_construct_image_surface);
  seed_object_set_property (ctx, namespace_ref, "ImageSurface", constructor_ref);
}
