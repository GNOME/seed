#include <seed.h>
#include <cairo/cairo.h>

#include "seed-cairo.h"
#include "seed-cairo-image-surface.h"

#define CAIRO_SURFACE_PRIV(obj) ((cairo_surface_t *)seed_object_get_private(obj))

#define CHECK_SURFACE(obj, res) ({						\
    if (!seed_object_is_of_class (ctx, obj, seed_cairo_surface_class)){	\
      seed_make_exception (ctx, exception, "ArgumentError", "Object is not a Cairo Surface"); \
      return seed_make_##res (ctx);\
    }									\
    if (!seed_object_get_private (obj)){				\
      seed_make_exception (ctx, exception, "ArgumentError", "Cairo surface has been destroyed"); \
      return seed_make_##res (ctx);}})

#define CHECK_THIS(res) if (!seed_object_get_private (this_object)){	\
    seed_make_exception (ctx, exception, "ArgumentError", "Cairo surface has been destroyed"); \
      return seed_make_##res (ctx);}

  

SeedClass seed_cairo_surface_class;

SeedClass
seed_get_cairo_surface_class ()
{
  return seed_cairo_surface_class;
}

static void
seed_cairo_surface_finalize (SeedObject obj)
{
  cairo_surface_t *s = CAIRO_SURFACE_PRIV(obj);
  if (s)
    cairo_surface_destroy (s);
}

cairo_surface_t *
seed_object_to_cairo_surface (SeedContext ctx, SeedObject obj, SeedException *exception)
{
  if (seed_object_is_of_class (ctx, obj, seed_cairo_surface_class))
    return CAIRO_SURFACE_PRIV (obj);
  seed_make_exception (ctx, exception, "ArgumentError", "Object is not a Cairo Surface");
  return NULL;
}

SeedObject
seed_object_from_cairo_surface (SeedContext ctx, cairo_surface_t *surf)
{
  SeedObject jsobj;
  
  jsobj = cairo_surface_get_user_data (surf, seed_get_cairo_key());
  if (jsobj)
    return jsobj;
  
  jsobj = seed_make_object (ctx, seed_cairo_surface_class, surf);
  cairo_surface_set_user_data (surf, seed_get_cairo_key(), jsobj, seed_cairo_destroy_func);
  return jsobj;
}

static SeedValue
seed_cairo_surface_create_similar (SeedContext ctx,
				   SeedObject function,
				   SeedObject this_object,
				   gsize argument_count,
				   const SeedValue arguments[],
				   SeedException *exception)
{
  gint width, height;
  cairo_surface_t *surface, *ret;
  cairo_content_t content;
  CHECK_THIS(null);
  if (argument_count != 3)
    {
      EXPECTED_EXCEPTION("create_similar", "3 arguments");
    }
  
  surface = seed_object_to_cairo_surface (ctx, this_object, exception);
  if (!surface)
    return seed_make_undefined (ctx);
  content = seed_value_to_long (ctx, arguments[0], exception);
  width = seed_value_to_int (ctx, arguments[1], exception);
  height = seed_value_to_int (ctx, arguments[2], exception);
  
  ret = cairo_surface_create_similar (surface, content, width, height);
  return seed_object_from_cairo_surface (ctx, ret);
}

static SeedValue
seed_cairo_surface_status (SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   gsize argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
  CHECK_THIS(null);
  return seed_value_from_long (ctx, cairo_surface_status (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedValue
seed_cairo_surface_get_content (SeedContext ctx,
				SeedObject function,
				SeedObject this_object,
				gsize argument_count,
				const SeedValue arguments[],
				SeedException *exception)
{
  CHECK_THIS(null);
  return seed_value_from_long (ctx, cairo_surface_get_content (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedValue
seed_cairo_surface_finish (SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   gsize argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
  CHECK_THIS(undefined);
  cairo_surface_finish (seed_object_to_cairo_surface(ctx, this_object, exception));
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_surface_flush (SeedContext ctx,
			  SeedObject function,
			  SeedObject this_object,
			  gsize argument_count,
			  const SeedValue arguments[],
			  SeedException *exception)
{
  CHECK_THIS(null);
  cairo_surface_flush (seed_object_to_cairo_surface(ctx, this_object, exception));
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_surface_mark_dirty_rectangle(SeedContext ctx,
					SeedObject function,
					SeedObject this_object,
					gsize argument_count,
					const SeedValue arguments[],
					SeedException *exception)
{
  cairo_surface_t *surf;
  guint x, y, width, height;
  CHECK_THIS(undefined);
  if (argument_count != 4)
    {
      EXPECTED_EXCEPTION("mark_dirty_rectangle", "4 arguments");
    }
  surf = seed_object_to_cairo_surface (ctx, this_object, exception);
  x = seed_value_to_int (ctx, arguments[0], exception);
  y = seed_value_to_int (ctx, arguments[1], exception);
  width = seed_value_to_int (ctx, arguments[2], exception);
  height = seed_value_to_int (ctx, arguments[3], exception);
  
  cairo_surface_mark_dirty_rectangle (surf, x, y, width, height);

  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_surface_mark_dirty (SeedContext ctx,
			       SeedObject function,
			       SeedObject this_object,
			       gsize argument_count,
			       const SeedValue arguments[],
			       SeedException *exception)
{
  CHECK_THIS(undefined);
  cairo_surface_mark_dirty (seed_object_to_cairo_surface(ctx, this_object, exception));
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_surface_set_device_offset(SeedContext ctx,
				     SeedObject function,
				     SeedObject this_object,
				     gsize argument_count,
				     const SeedValue arguments[],
				     SeedException *exception)
{
  cairo_surface_t *surf;
  gdouble x, y;
  CHECK_THIS(null);
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("set_device_offset", "2 arguments");
    }
  surf = seed_object_to_cairo_surface (ctx, this_object, exception);
  x = seed_value_to_double (ctx, arguments[0], exception);
  y = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_surface_set_device_offset (surf, x, y);

  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_surface_get_device_offset(SeedContext ctx,
				     SeedObject function,
				     SeedObject this_object,
				     gsize argument_count,
				     const SeedValue arguments[],
				     SeedException *exception)
{
  SeedValue offsets[2];
  cairo_surface_t *surf;
  gdouble x, y;
  CHECK_THIS(null);
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("get_device_offset", "no arguments");
    }
  surf = seed_object_to_cairo_surface (ctx, this_object, exception);
  cairo_surface_get_device_offset (surf, &x, &y);
  
  offsets[0] = seed_value_from_double (ctx, x, exception);
  offsets[1] = seed_value_from_double (ctx, y, exception);

  return seed_make_array (ctx, offsets, 2, exception);
}

static SeedValue
seed_cairo_surface_set_fallback_resolution(SeedContext ctx,
				     SeedObject function,
				     SeedObject this_object,
				     gsize argument_count,
				     const SeedValue arguments[],
				     SeedException *exception)
{
  cairo_surface_t *surf;
  gdouble x, y;
  CHECK_THIS(null);
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("set_fallback_resolution", "2 arguments");
    }
  surf = seed_object_to_cairo_surface (ctx, this_object, exception);
  x = seed_value_to_double (ctx, arguments[0], exception);
  y = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_surface_set_fallback_resolution (surf, x, y);

  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_surface_get_fallback_resolution(SeedContext ctx,
				     SeedObject function,
				     SeedObject this_object,
				     gsize argument_count,
				     const SeedValue arguments[],
				     SeedException *exception)
{
  SeedValue offsets[2];
  cairo_surface_t *surf;
  gdouble x, y;
  CHECK_THIS(null);
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("get_fallback_resolution", "no arguments");
    }
  surf = seed_object_to_cairo_surface (ctx, this_object, exception);
  cairo_surface_get_fallback_resolution (surf, &x, &y);
  
  offsets[0] = seed_value_from_double (ctx, x, exception);
  offsets[1] = seed_value_from_double (ctx, y, exception);

  return seed_make_array (ctx, offsets, 2, exception);
}

static SeedValue
seed_cairo_surface_get_type (SeedContext ctx,
				SeedObject function,
				SeedObject this_object,
				gsize argument_count,
				const SeedValue arguments[],
				SeedException *exception)
{
  CHECK_THIS(null);
  return seed_value_from_long (ctx, cairo_surface_get_type (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedValue
seed_cairo_surface_copy_page (SeedContext ctx,
			       SeedObject function,
			       SeedObject this_object,
			       gsize argument_count,
			       const SeedValue arguments[],
			       SeedException *exception)
{
  CHECK_THIS(undefined);
  cairo_surface_copy_page (seed_object_to_cairo_surface(ctx, this_object, exception));
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_surface_show_page (SeedContext ctx,
			       SeedObject function,
			       SeedObject this_object,
			       gsize argument_count,
			       const SeedValue arguments[],
			       SeedException *exception)
{
  CHECK_THIS(undefined);
  cairo_surface_show_page (seed_object_to_cairo_surface(ctx, this_object, exception));
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_surface_has_show_text_glyphs(SeedContext ctx,
					SeedObject function,
					SeedObject this_object,
					gsize argument_count,
					const SeedValue arguments[],
					SeedException *exception)
{
  CHECK_THIS(null);
  return seed_value_from_boolean (ctx, 
				  cairo_surface_has_show_text_glyphs (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}


seed_static_function surface_funcs[] = {
  {"create_similar", seed_cairo_surface_create_similar, 0},
  {"status", seed_cairo_surface_status, 0},
  {"finish", seed_cairo_surface_finish, 0},
  {"flush", seed_cairo_surface_flush, 0},
  //  {"get_font_options", seed_cairo_surface_get_font_options, 0},
  {"get_content", seed_cairo_surface_get_content, 0},
  {"mark_dirty", seed_cairo_surface_mark_dirty, 0},
  {"mark_dirty_rectangle", seed_cairo_surface_mark_dirty_rectangle, 0},
  {"set_device_offset", seed_cairo_surface_set_device_offset, 0},
  {"get_device_offset", seed_cairo_surface_get_device_offset, 0},
  {"set_fallback_resolution", seed_cairo_surface_set_fallback_resolution, 0},
  {"get_fallback_resolution", seed_cairo_surface_get_fallback_resolution, 0},
  {"get_type", seed_cairo_surface_get_type, 0},
  {"copy_page", seed_cairo_surface_copy_page, 0},
  {"show_page", seed_cairo_surface_show_page, 0},
  {"has_show_text_glyphs", seed_cairo_surface_has_show_text_glyphs, 0},
  {0,0,0}
};

void
seed_define_cairo_surface (SeedContext ctx,
			   SeedObject namespace_ref)
{
  seed_class_definition surface_def = seed_empty_class;
  
  surface_def.class_name = "CairoSurface";
  surface_def.finalize = seed_cairo_surface_finalize;
  surface_def.static_functions = surface_funcs;

  seed_cairo_surface_class = seed_create_class (&surface_def);
  
  seed_define_cairo_image_surface (ctx, namespace_ref);
}
