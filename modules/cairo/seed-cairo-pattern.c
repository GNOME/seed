#include <seed.h>
#include <cairo/cairo.h>

#include "seed-cairo.h"
#include "seed-cairo-matrix.h"

#define CAIRO_PATTERN_PRIV(obj) ((cairo_pattern_t *)seed_object_get_private(obj))

#define CHECK_PATTERN(obj, res) ({					\
      if (!seed_object_is_of_class (ctx, obj, seed_cairo_pattern_class)){ \
	seed_make_exception (ctx, exception, "ArgumentError", "Object is not a Cairo Pattern"); \
	return seed_make_##res (ctx);					\
      }									\
      if (!seed_object_get_private (obj)){				\
	seed_make_exception (ctx, exception, "ArgumentError", "Cairo pattern has been destroyed"); \
	return seed_make_##res (ctx);}})

#define CHECK_THIS() if (!seed_object_get_private (this_object)){	\
    seed_make_exception (ctx, exception, "ArgumentError", "Cairo pattern has been destroyed"); \
    return seed_make_undefined (ctx);}

#define CHECK_THIS_BOOL(res) if (!seed_object_get_private (this_object)){ \
    seed_make_exception (ctx, exception, "ArgumentError", "Cairo pattern has been destroyed"); return FALSE;} 


  

SeedClass seed_cairo_pattern_class;

SeedClass
seed_get_cairo_pattern_class ()
{
  return seed_cairo_pattern_class;
}

static void
seed_cairo_pattern_finalize (SeedObject obj)
{
  cairo_pattern_t *s = CAIRO_PATTERN_PRIV(obj);
  if (s)
    cairo_pattern_destroy (s);
}

cairo_pattern_t *
seed_object_to_cairo_pattern (SeedContext ctx, SeedObject obj, SeedException *exception)
{
  if (seed_object_is_of_class (ctx, obj, seed_cairo_pattern_class))
    return CAIRO_PATTERN_PRIV (obj);
  seed_make_exception (ctx, exception, "ArgumentError", "Object is not a Cairo Pattern");
  return NULL;
}

SeedObject
seed_object_from_cairo_pattern (SeedContext ctx, cairo_pattern_t *pat)
{
  SeedObject jsobj;
  
  jsobj = cairo_pattern_get_user_data (pat, seed_get_cairo_key());
  if (jsobj)
    return jsobj;
  
  jsobj = seed_make_object (ctx, seed_cairo_pattern_class, pat);
  cairo_pattern_set_user_data (pat, seed_get_cairo_key(), jsobj, seed_cairo_destroy_func);
  return jsobj;
}

static SeedObject
seed_cairo_construct_linear_gradient (SeedContext ctx,
				      SeedObject constructor,
				      size_t argument_count,
				      const SeedValue arguments[],
				      SeedException * exception)
{
  gdouble x0,y0,x1,y1;

  if (argument_count != 4)
    {
      EXPECTED_EXCEPTION("LinearGradient constructor", "4 arguments");
    }
  
  x0 = seed_value_to_double (ctx, arguments[0], exception);
  y0 = seed_value_to_double (ctx, arguments[1], exception);
  x1 = seed_value_to_double (ctx, arguments[2], exception);
  y1 = seed_value_to_double (ctx, arguments[3], exception);
  
  return seed_object_from_cairo_pattern (ctx, cairo_pattern_create_linear (x0, y0, x1, y1));
}

static SeedObject
seed_cairo_construct_radial_gradient (SeedContext ctx,
				      SeedObject constructor,
				      size_t argument_count,
				      const SeedValue arguments[],
				      SeedException * exception)
{
  gdouble cx0, cy0, r0, cx1, cy1, r1;

  if (argument_count != 6)
    {
      EXPECTED_EXCEPTION("RadialGradient constructor", "6 arguments");
    }
  
  cx0 = seed_value_to_double (ctx, arguments[0], exception);
  cy0 = seed_value_to_double (ctx, arguments[1], exception);
  r0 = seed_value_to_double (ctx, arguments[2], exception);
  cx1 = seed_value_to_double (ctx, arguments[3], exception);
  cy1 = seed_value_to_double (ctx, arguments[4], exception);
  r1 = seed_value_to_double (ctx, arguments[5], exception);

  
  return seed_object_from_cairo_pattern (ctx, cairo_pattern_create_radial (cx0, cy0, r0, cx1, cy1, r1));
}

static SeedValue
seed_cairo_pattern_add_color_stop_rgb (SeedContext ctx,
				       SeedObject function,
				       SeedObject this_object,
				       gsize argument_count,
				       const SeedValue arguments[],
				       SeedException *exception)
{
  gdouble offset, r, g, b;
  cairo_pattern_t *pat;
  CHECK_THIS();
  if (argument_count != 4)
    {
      EXPECTED_EXCEPTION("add_color_stop_rgb", "4 arguments");
    }
  
  pat = seed_object_get_private (this_object);
  offset = seed_value_to_double (ctx, arguments[0], exception);
  r = seed_value_to_double (ctx, arguments[1], exception);
  g = seed_value_to_double (ctx, arguments[2], exception);
  b = seed_value_to_double (ctx, arguments[3], exception);
  
  cairo_pattern_add_color_stop_rgb (pat, offset, r, g, b);
  
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_pattern_add_color_stop_rgba (SeedContext ctx,
				       SeedObject function,
				       SeedObject this_object,
				       gsize argument_count,
				       const SeedValue arguments[],
				       SeedException *exception)
{
  gdouble offset, r, g, b, a;
  cairo_pattern_t *pat;
  CHECK_THIS();
  if (argument_count != 5)
    {
      EXPECTED_EXCEPTION("add_color_stop_rgba", "5 arguments");
    }
  
  pat = seed_object_get_private (this_object);
  offset = seed_value_to_double (ctx, arguments[0], exception);
  r = seed_value_to_double (ctx, arguments[1], exception);
  g = seed_value_to_double (ctx, arguments[2], exception);
  b = seed_value_to_double (ctx, arguments[3], exception);
  a = seed_value_to_double (ctx, arguments[4], exception);
  
  cairo_pattern_add_color_stop_rgba (pat, offset, r, g, b, a);
  
  return seed_make_undefined (ctx);
}

seed_static_function pattern_funcs[] = {
  {"add_color_stop_rgb", seed_cairo_pattern_add_color_stop_rgb, 0},
  {"add_color_stop_rgba", seed_cairo_pattern_add_color_stop_rgba, 0},
  {0,0,0}
};

void
seed_define_cairo_pattern (SeedContext ctx,
			   SeedObject namespace_ref)
{
  SeedObject linear_constructor, radial_constructor;
  seed_class_definition pattern_def = seed_empty_class;
  
  pattern_def.class_name = "Pattern";
  pattern_def.finalize = seed_cairo_pattern_finalize;
  pattern_def.static_functions = pattern_funcs;
  
  seed_cairo_pattern_class = seed_create_class (&pattern_def);
  
  linear_constructor = seed_make_constructor (ctx, NULL, seed_cairo_construct_linear_gradient);
  seed_object_set_property(ctx, namespace_ref, "LinearGradient", linear_constructor);

  radial_constructor = seed_make_constructor (ctx, NULL, seed_cairo_construct_radial_gradient);
  seed_object_set_property(ctx, namespace_ref, "RadialGradient", radial_constructor);
}
