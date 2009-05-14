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
seed_object_from_cairo_pattern (SeedContext ctx, cairo_pattern_t *surf)
{
  SeedObject jsobj;
  
  jsobj = cairo_pattern_get_user_data (surf, seed_get_cairo_key());
  if (jsobj)
    return jsobj;
  
  jsobj = seed_make_object (ctx, seed_cairo_pattern_class, surf);
  cairo_pattern_set_user_data (surf, seed_get_cairo_key(), jsobj, seed_cairo_destroy_func);
  return jsobj;
}

void
seed_define_cairo_pattern (SeedContext ctx,
			   SeedObject namespace_ref)
{
  seed_class_definition pattern_def = seed_empty_class;
  
  pattern_def.class_name = "Pattern";
  pattern_def.finalize = seed_cairo_pattern_finalize;
  
  seed_cairo_pattern_class = seed_create_class (&pattern_def);
}
