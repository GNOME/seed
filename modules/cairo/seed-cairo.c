#include <seed.h>
#include <cairo/cairo.h>
#include "seed-cairo.h"
#include "seed-cairo-surface.h"
#include "seed-cairo-image-surface.h"
#include "seed-cairo-enums.h"
#include "seed-cairo-matrix.h"
#include "seed-cairo-pattern.h"

SeedEngine *eng;

#define CAIRO_CONTEXT_PRIV(obj) ((cairo_t *)seed_object_get_private(obj))

#define CHECK_CAIRO(obj) ({						\
      if (!seed_object_is_of_class (ctx, obj, seed_cairo_class)){	\
	seed_make_exception (ctx, exception, "ArgumentError", "Object is not a Cairo Context"); \
	return seed_make_undefined (ctx);				\
      }									\
      if (!seed_object_get_private (obj)){				\
	seed_make_exception (ctx, exception, "ArgumentError", "Cairo Context has been destroyed"); \
	return seed_make_undefined (ctx);}})

#define CHECK_THIS() if (!seed_object_get_private (this_object)){	\
    seed_make_exception (ctx, exception, "ArgumentError", "Cairo Context has been destroyed"); \
    return seed_make_undefined (ctx);}

#define CHECK_THIS_BOOL() if (!seed_object_get_private (this_object)){	\
    seed_make_exception (ctx, exception, "ArgumentError", "Cairo Context has been destroyed"); \
    return FALSE;}

SeedClass seed_cairo_context_class;

cairo_user_data_key_t *
seed_get_cairo_key ()
{
  static cairo_user_data_key_t foobaz;
  
  return &foobaz;
}

void
seed_cairo_destroy_func (void *obj)
{
  SeedObject object = (SeedObject)obj;
  seed_object_set_private (object, NULL);
}

cairo_t *
seed_object_to_cairo_context (SeedContext ctx, SeedObject obj, SeedException *exception)
{
  if (seed_object_is_of_class (ctx, obj, seed_cairo_context_class))
    return CAIRO_CONTEXT_PRIV (obj);
  seed_make_exception (ctx, exception, "ArgumentError", "Object is not a Cairo Context");
  return NULL;
}

SeedObject
seed_object_from_cairo_context (SeedContext ctx, cairo_t *cr)
{
  SeedObject jsobj;
  
  jsobj = cairo_get_user_data (cr, seed_get_cairo_key());
  if (jsobj)
    return jsobj;
  
  jsobj = seed_make_object (ctx, seed_cairo_context_class, cr);
  cairo_set_user_data (cr, seed_get_cairo_key(), jsobj, seed_cairo_destroy_func);
  return jsobj;
}

void
seed_cairo_context_finalize (SeedObject obj)
{
  cairo_t *cr = CAIRO_CONTEXT_PRIV (obj);
  if (cr)
    cairo_destroy (cr);
}

static SeedObject
seed_cairo_construct_context (SeedContext ctx,
			      SeedObject constructor,
			      size_t argument_count,
			      const SeedValue arguments[],
			      SeedException * exception)
{
  cairo_surface_t *surf;
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION ("Context", "1 argument");
    }
  surf = seed_object_to_cairo_surface (ctx, arguments[0], exception);
  if (!surf)
    return seed_make_undefined (ctx);
  return seed_object_from_cairo_context (ctx, cairo_create (surf));
}

static SeedValue 
seed_cairo_save (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 gsize argument_count,
		 const SeedValue arguments[],
		 SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_save(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_restore (SeedContext ctx,
		    SeedObject function,
		    SeedObject this_object,
		    gsize argument_count,
		    const SeedValue arguments[],
		    SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_restore(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_get_target (SeedContext ctx,
		       SeedObject this_object,
		       SeedString property_name,
		       SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  return seed_object_from_cairo_surface (ctx, cairo_get_target (cr));
}

static SeedValue 
seed_cairo_push_group (SeedContext ctx,
		       SeedObject function,
		       SeedObject this_object,
		       gsize argument_count,
		       const SeedValue arguments[],
		       SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_push_group(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_push_group_with_content (SeedContext ctx,
				    SeedObject function,
				    SeedObject this_object,
				    gsize argument_count,
				    const SeedValue arguments[],
				    SeedException *exception)
{
  cairo_content_t content;
  cairo_t *cr;
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("push_group_with_content", "1 argument");
    }
    
  CHECK_THIS();  
  cr = seed_object_get_private (this_object);
  content = seed_value_to_long (ctx, arguments[0], exception);
  cairo_push_group_with_content(cr, content);

  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_pop_group_to_source (SeedContext ctx,
				SeedObject function,
				SeedObject this_object,
				gsize argument_count,
				const SeedValue arguments[],
				SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_pop_group_to_source(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_get_group_target (SeedContext ctx,
			     SeedObject function,
			     SeedObject this_object,
			     gsize argument_count,
			     const SeedValue arguments[],
			     SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  return seed_object_from_cairo_surface (ctx, cairo_get_group_target (cr));
}

static SeedValue
seed_cairo_set_source_rgb(SeedContext ctx,
			  SeedObject function,
			  SeedObject this_object,
			  gsize argument_count,
			  const SeedValue arguments[],
			  SeedException *exception)
{
  gdouble r,g,b;
  cairo_t *cr;

  CHECK_THIS();
  cr = seed_object_get_private (this_object);
  
  if (argument_count != 3)
    {
      EXPECTED_EXCEPTION("set_source_rgb", "3 arguments");
    }
  r = seed_value_to_double (ctx, arguments[0], exception);
  g = seed_value_to_double (ctx, arguments[1], exception);
  b = seed_value_to_double (ctx, arguments[2], exception);
  cairo_set_source_rgb (cr, r, g, b);
  
  return seed_make_undefined (ctx);  
}

static SeedValue
seed_cairo_set_source_rgba(SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   gsize argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
  gdouble r,g,b,a;
  cairo_t *cr;

  CHECK_THIS();
  cr = seed_object_get_private (this_object);
  
  if (argument_count != 4)
    {
      EXPECTED_EXCEPTION("set_source_rgba", "4 arguments");
    }
  r = seed_value_to_double (ctx, arguments[0], exception);
  g = seed_value_to_double (ctx, arguments[1], exception);
  b = seed_value_to_double (ctx, arguments[2], exception);
  a = seed_value_to_double (ctx, arguments[3], exception);
  cairo_set_source_rgba (cr, r, g, b, a);
  
  return seed_make_undefined (ctx);  
}

static SeedValue
seed_cairo_set_source_surface (SeedContext ctx,
			       SeedObject function,
			       SeedObject this_object,
			       gsize argument_count,
			       const SeedValue arguments[],
			       SeedException *exception)
{
  gdouble x,y;
  cairo_surface_t *surface;
  cairo_t *cr;
  
  CHECK_THIS();
  if (argument_count != 3)
    {
      EXPECTED_EXCEPTION("set_source_surface", "3 arguments");
    }
  cr = seed_object_get_private (this_object);
  surface = seed_object_to_cairo_surface (ctx, arguments[0], exception);
  if (!surface)
    return seed_make_undefined (ctx);
  
  x = seed_value_to_double (ctx, arguments[1], exception);
  y = seed_value_to_double (ctx, arguments[2], exception);
  cairo_set_source_surface (cr, surface, x, y);
  
  return seed_make_undefined (ctx);
}

static gboolean
seed_cairo_set_antialias (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedValue value,
			  SeedException *exception)
{
  cairo_t *cr;
  cairo_antialias_t antialias;
  
  CHECK_THIS_BOOL();
  cr = seed_object_get_private (this_object);
  antialias = seed_value_to_long (ctx, value, exception);

  cairo_set_antialias (cr, antialias);
  return TRUE;
}

static SeedValue
seed_cairo_get_antialias (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedException *exception)
{
  cairo_antialias_t antialias;
  cairo_t *cr;
  
  CHECK_THIS();
  cr = seed_object_get_private (this_object);
  antialias = cairo_get_antialias (cr);
  
  return seed_value_from_long (ctx, antialias, exception);
}

static SeedValue
seed_cairo_set_dash(SeedContext ctx,
		    SeedObject function,
		    SeedObject this_object,
		    gsize argument_count,
		    const SeedValue arguments[],
		    SeedException *exception)
{
  SeedValue length;
  cairo_t *cr;
  gdouble *dashes, offset;
  gint num_dashes, i;

  CHECK_THIS();
  cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("set_dash", "2 arguments");
    }
  length = seed_object_get_property (ctx, arguments[0], "length");
  num_dashes = seed_value_to_int (ctx, length, exception);
  dashes = g_alloca (num_dashes * sizeof(gdouble));
  for (i = 0; i < num_dashes; i++)
    {
      dashes[i] = seed_value_to_double(ctx,
				       seed_object_get_property_at_index (ctx,
									  arguments[0],
									  i,
									  exception),
				       exception);

    }
  offset = seed_value_to_double (ctx, arguments[1], exception);
  cairo_set_dash (cr, dashes, num_dashes, offset);
  
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_get_dash_count (SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   gsize argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
  cairo_t *cr;
  gint dash_count;
  CHECK_THIS();
  
  cr = seed_object_get_private (this_object);
  dash_count = cairo_get_dash_count (cr);
  
  return seed_value_from_int (ctx, dash_count, exception);
}

static SeedValue
seed_cairo_get_dash (SeedContext ctx,
		     SeedObject function,
		     SeedObject this_object,
		     gsize argument_count,
		     const SeedValue arguments[],
		     SeedException *exception)
{
  SeedValue ret[2], *jsdashes;
  cairo_t *cr;
  gint dash_count, i;
  gdouble *dashes, offset;
  
  CHECK_THIS();
  cr = seed_object_get_private (this_object);
  dash_count = cairo_get_dash_count (cr);
  dashes = g_alloca (dash_count * sizeof(gdouble));
  jsdashes = g_alloca (dash_count * sizeof(SeedValue));
  
  cairo_get_dash (cr, dashes, &offset);
  for (i = 0; i < dash_count; i++)
    {
      jsdashes[i] = seed_value_from_double (ctx, dashes[i], exception);
    }
  ret[0] = seed_make_array (ctx, jsdashes, dash_count, exception);
  ret[1] = seed_value_from_double (ctx, offset, exception);
  
  return seed_make_array (ctx, ret, 2, exception);
}

static SeedValue
seed_cairo_get_fill_rule (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS();
  
  cr = seed_object_get_private (this_object);
  return seed_value_from_long (ctx, cairo_get_fill_rule (cr), exception);
}

static gboolean
seed_cairo_set_fill_rule (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedValue value,
			  SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS_BOOL();
  
  cr = seed_object_get_private (this_object);
  cairo_set_fill_rule (cr, seed_value_to_long (ctx, value, exception));

  return TRUE;
}


static SeedValue
seed_cairo_get_line_cap (SeedContext ctx,
			 SeedObject this_object,
			 SeedString property_name,
			 SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS();
  
  cr = seed_object_get_private (this_object);
  return seed_value_from_long (ctx, cairo_get_line_cap (cr), exception);
}

static gboolean
seed_cairo_set_line_cap (SeedContext ctx,
			 SeedObject this_object,
			 SeedString property_name,
			 SeedValue value,
			 SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS_BOOL();
  
  cr = seed_object_get_private (this_object);
  cairo_set_line_cap (cr, seed_value_to_long (ctx, value, exception));

  return TRUE;
}

static SeedValue
seed_cairo_get_line_join (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS();
  
  cr = seed_object_get_private (this_object);
  return seed_value_from_long (ctx, cairo_get_line_join (cr), exception);
}

static gboolean
seed_cairo_set_line_join (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedValue value,
			  SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS_BOOL();
  
  cr = seed_object_get_private (this_object);
  cairo_set_line_join (cr, seed_value_to_long (ctx, value, exception));
  
  return TRUE;
}


static SeedValue
seed_cairo_get_line_width (SeedContext ctx,
			   SeedObject this_object,
			   SeedString property_name,
			   SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS();
  
  cr = seed_object_get_private (this_object);
  return seed_value_from_double (ctx, cairo_get_line_width (cr), exception);
}

static gboolean
seed_cairo_set_line_width (SeedContext ctx,
			   SeedObject this_object,
			   SeedString property_name,
			   SeedValue value,
			   SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS_BOOL();
  
  cr = seed_object_get_private (this_object);
  cairo_set_line_width (cr, seed_value_to_double (ctx, value, exception));
  
  return TRUE;
}

static SeedValue
seed_cairo_get_miter_limit (SeedContext ctx,
			    SeedObject this_object,
			    SeedString property_name,
			    SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS();
  
  cr = seed_object_get_private (this_object);
  return seed_value_from_double (ctx, cairo_get_miter_limit (cr), exception);
}

static gboolean
seed_cairo_set_miter_limit (SeedContext ctx,
			    SeedObject this_object,
			    SeedString property_name,
			    SeedValue value,
			    SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS_BOOL();
  
  cr = seed_object_get_private (this_object);
  cairo_set_miter_limit (cr, seed_value_to_double (ctx, value, exception));
  
  return TRUE;
}

static SeedValue
seed_cairo_get_operator (SeedContext ctx,
			 SeedObject this_object,
			 SeedString property_name,
			 SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS();
  
  cr = seed_object_get_private (this_object);
  return seed_value_from_long (ctx, cairo_get_operator (cr), exception);
}

static gboolean
seed_cairo_set_operator (SeedContext ctx,
			 SeedObject this_object,
			 SeedString property_name,
			 SeedValue value,
			 SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS_BOOL();
  
  cr = seed_object_get_private (this_object);
  cairo_set_operator (cr, seed_value_to_long (ctx, value, exception));
  
  return TRUE;
}

static SeedValue
seed_cairo_get_tolerance (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS();
  
  cr = seed_object_get_private (this_object);
  return seed_value_from_double (ctx, cairo_get_tolerance (cr), exception);
}

static gboolean
seed_cairo_set_tolerance (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedValue value,
			  SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS_BOOL();
  
  cr = seed_object_get_private (this_object);
  cairo_set_tolerance (cr, seed_value_to_double (ctx, value, exception));
  
  return TRUE;
}

static SeedValue 
seed_cairo_clip (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 gsize argument_count,
		 const SeedValue arguments[],
		 SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_clip(cr);
  return seed_make_undefined (ctx);
}


static SeedValue 
seed_cairo_clip_preserve (SeedContext ctx,
			  SeedObject function,
			  SeedObject this_object,
			  gsize argument_count,
			  const SeedValue arguments[],
			  SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_clip_preserve(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_reset_clip (SeedContext ctx,
		       SeedObject function,
		       SeedObject this_object,
		       gsize argument_count,
		       const SeedValue arguments[],
		       SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_reset_clip(cr);
  return seed_make_undefined (ctx);
}


static SeedValue 
seed_cairo_clip_extents (SeedContext ctx,
			 SeedObject function,
			 SeedObject this_object,
			 gsize argument_count,
			 const SeedValue arguments[],
			 SeedException *exception)
{
  SeedValue jsextents[4];
  gdouble extents[4];
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 4)
    {
      EXPECTED_EXCEPTION("clip_extents", "4 arguments");
    }
  
  cairo_clip_extents (cr, &extents[0], &extents[1], &extents[2], &extents[3]);
  jsextents[0] = seed_value_from_double (ctx, extents[0], exception);
  jsextents[1] = seed_value_from_double (ctx, extents[1], exception);
  jsextents[2] = seed_value_from_double (ctx, extents[2], exception);
  jsextents[3] = seed_value_from_double (ctx, extents[3], exception);

  return seed_make_array (ctx, jsextents, 4, exception);
}


static SeedValue 
seed_cairo_fill (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 gsize argument_count,
		 const SeedValue arguments[],
		 SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_fill(cr);
  return seed_make_undefined (ctx);
}


static SeedValue 
seed_cairo_fill_preserve (SeedContext ctx,
			  SeedObject function,
			  SeedObject this_object,
			  gsize argument_count,
			  const SeedValue arguments[],
			  SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_fill_preserve(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_fill_extents (SeedContext ctx,
			 SeedObject function,
			 SeedObject this_object,
			 gsize argument_count,
			 const SeedValue arguments[],
			 SeedException *exception)
{
  SeedValue jsextents[4];
  gdouble extents[4];
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 4)
    {
      EXPECTED_EXCEPTION("clip_extents", "4 arguments");
    }
  
  cairo_fill_extents (cr, &extents[0], &extents[1], &extents[2], &extents[3]);
  jsextents[0] = seed_value_from_double (ctx, extents[0], exception);
  jsextents[1] = seed_value_from_double (ctx, extents[1], exception);
  jsextents[2] = seed_value_from_double (ctx, extents[2], exception);
  jsextents[3] = seed_value_from_double (ctx, extents[3], exception);

  return seed_make_array (ctx, jsextents, 4, exception);
}

static SeedValue
seed_cairo_in_fill (SeedContext ctx,
		    SeedObject function,
		    SeedObject this_object,
		    gsize argument_count,
		    const SeedValue arguments[],
		    SeedException *exception)
{
  gdouble x, y;
  cairo_t *cr;
  CHECK_THIS();
  cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("in_fill", "2 arguments");
    }
  
  x = seed_value_to_double (ctx, arguments[0], exception);
  y = seed_value_to_double (ctx, arguments[1], exception);
  
  return seed_value_from_boolean (ctx, cairo_in_fill (cr, x, y), exception);
}

static SeedValue
seed_cairo_mask_surface (SeedContext ctx,
			 SeedObject function,
			 SeedObject this_object,
			 gsize argument_count,
			 const SeedValue arguments[],
			 SeedException *exception)
{
  gdouble x,y;
  cairo_surface_t *surface;
  cairo_t *cr;
  
  CHECK_THIS();
  if (argument_count != 3)
    {
      EXPECTED_EXCEPTION("mask_surface", "3 arguments");
    }
  cr = seed_object_get_private (this_object);
  surface = seed_object_to_cairo_surface (ctx, arguments[0], exception);
  if (!surface)
    return seed_make_undefined (ctx);
  
  x = seed_value_to_double (ctx, arguments[1], exception);
  y = seed_value_to_double (ctx, arguments[2], exception);
  cairo_mask_surface (cr, surface, x, y);
  
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_paint (SeedContext ctx,
		  SeedObject function,
		  SeedObject this_object,
		  gsize argument_count,
		  const SeedValue arguments[],
		  SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_paint(cr);
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_paint_with_alpha (SeedContext ctx,
			     SeedObject function,
			     SeedObject this_object,
			     gsize argument_count,
			     const SeedValue arguments[],
			     SeedException *exception)
{
  cairo_t *cr;
  CHECK_THIS();
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("paint_with_alpha", "1 argument");
    }
  
  cr = seed_object_get_private (this_object);
  cairo_paint_with_alpha (cr, seed_value_to_double (ctx, arguments[0], exception));
  
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_stroke (SeedContext ctx,
		   SeedObject function,
		   SeedObject this_object,
		   gsize argument_count,
		   const SeedValue arguments[],
		   SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_stroke(cr);
  return seed_make_undefined (ctx);
}


static SeedValue 
seed_cairo_stroke_preserve (SeedContext ctx,
			    SeedObject function,
			    SeedObject this_object,
			    gsize argument_count,
			    const SeedValue arguments[],
			    SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_stroke_preserve(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_stroke_extents (SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   gsize argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
  SeedValue jsextents[4];
  gdouble extents[4];
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 4)
    {
      EXPECTED_EXCEPTION("clip_extents", "4 arguments");
    }
  
  cairo_stroke_extents (cr, &extents[0], &extents[1], &extents[2], &extents[3]);
  jsextents[0] = seed_value_from_double (ctx, extents[0], exception);
  jsextents[1] = seed_value_from_double (ctx, extents[1], exception);
  jsextents[2] = seed_value_from_double (ctx, extents[2], exception);
  jsextents[3] = seed_value_from_double (ctx, extents[3], exception);

  return seed_make_array (ctx, jsextents, 4, exception);
}

static SeedValue
seed_cairo_in_stroke (SeedContext ctx,
		      SeedObject function,
		      SeedObject this_object,
		      gsize argument_count,
		      const SeedValue arguments[],
		      SeedException *exception)
{
  gdouble x, y;
  cairo_t *cr;
  CHECK_THIS();
  cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("in_stroke", "2 arguments");
    }
  
  x = seed_value_to_double (ctx, arguments[0], exception);
  y = seed_value_to_double (ctx, arguments[1], exception);
  
  return seed_value_from_boolean (ctx, cairo_in_stroke (cr, x, y), exception);
}

static SeedValue 
seed_cairo_copy_page (SeedContext ctx,
		      SeedObject function,
		      SeedObject this_object,
		      gsize argument_count,
		      const SeedValue arguments[],
		      SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_copy_page(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_show_page (SeedContext ctx,
		      SeedObject function,
		      SeedObject this_object,
		      gsize argument_count,
		      const SeedValue arguments[],
		      SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_show_page(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_has_current_point (SeedContext ctx,
			      SeedObject function,
			      SeedObject this_object,
			      gsize argument_count,
			      const SeedValue arguments[],
			      SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  return seed_value_from_boolean (ctx, cairo_has_current_point(cr), exception);
}

static SeedValue 
seed_cairo_get_current_point (SeedContext ctx,
			      SeedObject function,
			      SeedObject this_object,
			      gsize argument_count,
			      const SeedValue arguments[],
			      SeedException *exception)
{
  SeedValue points[2];
  gdouble x, y;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);

  cairo_get_current_point (cr, &x, &y);
  points[0] = seed_value_from_double (ctx, x, exception);
  points[1] = seed_value_from_double (ctx, y, exception);
  
  return seed_make_array (ctx, points, 2, exception);
}

static SeedValue 
seed_cairo_new_path (SeedContext ctx,
		     SeedObject function,
		     SeedObject this_object,
		     gsize argument_count,
		     const SeedValue arguments[],
		     SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_new_path(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_new_sub_path (SeedContext ctx,
			 SeedObject function,
			 SeedObject this_object,
			 gsize argument_count,
			 const SeedValue arguments[],
			 SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_new_sub_path(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_close_path (SeedContext ctx,
		       SeedObject function,
		       SeedObject this_object,
		       gsize argument_count,
		       const SeedValue arguments[],
		       SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_close_path(cr);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_arc (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		gsize argument_count,
		const SeedValue arguments[],
		SeedException *exception)
{
  gdouble xc, yc, radius, angle1, angle2;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 5)
    {
      EXPECTED_EXCEPTION ("arc", "5 arguments");
    }
  xc = seed_value_to_double (ctx, arguments[0], exception);
  yc = seed_value_to_double (ctx, arguments[1], exception);
  radius = seed_value_to_double (ctx, arguments[2], exception);
  angle1 = seed_value_to_double (ctx, arguments[3], exception);
  angle2 = seed_value_to_double (ctx, arguments[4], exception);
  
  cairo_arc (cr, xc, yc, radius, angle1, angle2);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_arc_negative (SeedContext ctx,
			 SeedObject function,
			 SeedObject this_object,
			 gsize argument_count,
			 const SeedValue arguments[],
			 SeedException *exception)
{
  gdouble xc, yc, radius, angle1, angle2;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 5)
    {
      EXPECTED_EXCEPTION ("arc_negative", "5 arguments");
    }
  xc = seed_value_to_double (ctx, arguments[0], exception);
  yc = seed_value_to_double (ctx, arguments[1], exception);
  radius = seed_value_to_double (ctx, arguments[2], exception);
  angle1 = seed_value_to_double (ctx, arguments[3], exception);
  angle2 = seed_value_to_double (ctx, arguments[4], exception);
  
  cairo_arc_negative (cr, xc, yc, radius, angle1, angle2);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_curve_to (SeedContext ctx,
		     SeedObject function,
		     SeedObject this_object,
		     gsize argument_count,
		     const SeedValue arguments[],
		     SeedException *exception)
{
  gdouble x1,y1,x2,y2,x3,y3;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 6)
    {
      EXPECTED_EXCEPTION ("curve_to", "6 arguments");
    }
  x1 = seed_value_to_double (ctx, arguments[0], exception);
  y1 = seed_value_to_double (ctx, arguments[1], exception);
  x2 = seed_value_to_double (ctx, arguments[2], exception);
  y2 = seed_value_to_double (ctx, arguments[3], exception);
  x3 = seed_value_to_double (ctx, arguments[4], exception);
  y3 = seed_value_to_double (ctx, arguments[5], exception);
  
  cairo_curve_to (cr, x1, y2, x2, y2, x3, y3);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_rel_curve_to (SeedContext ctx,
			 SeedObject function,
			 SeedObject this_object,
			 gsize argument_count,
			 const SeedValue arguments[],
			 SeedException *exception)
{
  gdouble x1,y1,x2,y2,x3,y3;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 6)
    {
      EXPECTED_EXCEPTION ("rel_curve_to", "6 arguments");
    }
  x1 = seed_value_to_double (ctx, arguments[0], exception);
  y1 = seed_value_to_double (ctx, arguments[1], exception);
  x2 = seed_value_to_double (ctx, arguments[2], exception);
  y2 = seed_value_to_double (ctx, arguments[3], exception);
  x3 = seed_value_to_double (ctx, arguments[4], exception);
  y3 = seed_value_to_double (ctx, arguments[5], exception);
  
  cairo_rel_curve_to (cr, x1, y2, x2, y2, x3, y3);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_line_to (SeedContext ctx,
		    SeedObject function,
		    SeedObject this_object,
		    gsize argument_count,
		    const SeedValue arguments[],
		    SeedException *exception)
{
  gdouble x1,y1;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("line_to", "2 arguments");
    }
  x1 = seed_value_to_double (ctx, arguments[0], exception);
  y1 = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_line_to (cr, x1, y1);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_rel_line_to (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			gsize argument_count,
			const SeedValue arguments[],
			SeedException *exception)
{
  gdouble x1,y1;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("rel_line_to", "2 arguments");
    }
  x1 = seed_value_to_double (ctx, arguments[0], exception);
  y1 = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_rel_line_to (cr, x1, y1);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_move_to (SeedContext ctx,
		    SeedObject function,
		    SeedObject this_object,
		    gsize argument_count,
		    const SeedValue arguments[],
		    SeedException *exception)
{
  gdouble x1,y1;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("move_to", "2 arguments");
    }
  x1 = seed_value_to_double (ctx, arguments[0], exception);
  y1 = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_move_to (cr, x1, y1);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_rel_move_to (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			gsize argument_count,
			const SeedValue arguments[],
			SeedException *exception)
{
  gdouble x1,y1;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("rel_move_to", "2 arguments");
    }
  x1 = seed_value_to_double (ctx, arguments[0], exception);
  y1 = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_rel_move_to (cr, x1, y1);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_rectangle (SeedContext ctx,
		      SeedObject function,
		      SeedObject this_object,
		      gsize argument_count,
		      const SeedValue arguments[],
		      SeedException *exception)
{
  gdouble x1,y1, width, height;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 4)
    {
      EXPECTED_EXCEPTION ("rectangle", "2 arguments");
    }
  x1 = seed_value_to_double (ctx, arguments[0], exception);
  y1 = seed_value_to_double (ctx, arguments[1], exception);
  width = seed_value_to_double (ctx, arguments[2], exception);
  height = seed_value_to_double (ctx, arguments[3], exception);
  
  cairo_rectangle (cr, x1, y1, width, height);
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_text_path (SeedContext ctx,
		      SeedObject function,
		      SeedObject this_object,
		      gsize argument_count,
		      const SeedValue arguments[],
		      SeedException *exception)
{
  cairo_t *cr;
  gchar *text;
  CHECK_THIS();
  cr = seed_object_get_private (this_object);
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("text_path", "1 argument");
    }
  text = seed_value_to_string (ctx, arguments[0], exception);
  cairo_text_path (cr, text);
  g_free (text);
  
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_path_extents (SeedContext ctx,
			 SeedObject function,
			 SeedObject this_object,
			 gsize argument_count,
			 const SeedValue arguments[],
			 SeedException *exception)
{
  SeedValue jsextents[4];
  gdouble extents[4];
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 4)
    {
      EXPECTED_EXCEPTION("path_extents", "4 arguments");
    }
  
  cairo_path_extents (cr, &extents[0], &extents[1], &extents[2], &extents[3]);
  jsextents[0] = seed_value_from_double (ctx, extents[0], exception);
  jsextents[1] = seed_value_from_double (ctx, extents[1], exception);
  jsextents[2] = seed_value_from_double (ctx, extents[2], exception);
  jsextents[3] = seed_value_from_double (ctx, extents[3], exception);

  return seed_make_array (ctx, jsextents, 4, exception);
}

static SeedValue 
seed_cairo_translate (SeedContext ctx,
		      SeedObject function,
		      SeedObject this_object,
		      gsize argument_count,
		      const SeedValue arguments[],
		      SeedException *exception)
{
  gdouble x1,y1;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("translate", "2 arguments");
    }
  x1 = seed_value_to_double (ctx, arguments[0], exception);
  y1 = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_translate (cr, x1, y1);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_scale (SeedContext ctx,
		  SeedObject function,
		  SeedObject this_object,
		  gsize argument_count,
		  const SeedValue arguments[],
		  SeedException *exception)
{
  gdouble x1,y1;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("scale", "2 arguments");
    }
  x1 = seed_value_to_double (ctx, arguments[0], exception);
  y1 = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_scale (cr, x1, y1);
  return seed_make_undefined (ctx);
}

static SeedValue 
seed_cairo_rotate (SeedContext ctx,
		   SeedObject function,
		   SeedObject this_object,
		   gsize argument_count,
		   const SeedValue arguments[],
		   SeedException *exception)
{
  gdouble angle;
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("rotate", "1 arguments");
    }
  angle = seed_value_to_double (ctx, arguments[0], exception);

  
  cairo_rotate (cr, angle);
  return seed_make_undefined (ctx);
}




static SeedValue
seed_cairo_transform (SeedContext ctx,
		      SeedObject function,
		      SeedObject this_object,
		      gsize argument_count,
		      const SeedValue arguments[],
		      SeedException *exception)
{
  cairo_matrix_t matrix;
  cairo_t *cr;
  CHECK_THIS();

  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("transform", "1 argument");
    }
  if (!seed_value_to_cairo_matrix (ctx, arguments[0], &matrix, exception))
    {
      seed_make_exception (ctx, exception, "ArgumentError", "transform expects an array [xx,yx,xy,yy,x0,y0]");
      return seed_make_undefined (ctx);
    }
  cr = seed_object_get_private (this_object);

  cairo_transform (cr, &matrix);
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_get_matrix (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedException *exception)
{
  cairo_t *cr;
  cairo_matrix_t m;
  CHECK_THIS();
  
  cr = seed_object_get_private (this_object);
  cairo_get_matrix (cr, &m);
  return seed_value_from_cairo_matrix (ctx, &m, exception);
}

static gboolean
seed_cairo_set_matrix (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedValue value,
			  SeedException *exception)
{
  cairo_matrix_t m;
  cairo_t *cr;
  CHECK_THIS_BOOL();
  
  cr = seed_object_get_private (this_object);
  if (!seed_value_to_cairo_matrix (ctx, value, &m, exception))
    {
      seed_make_exception (ctx, exception, "ArgumentError", "matrix must be an array [xx,yx,xy,yy,x0,y0]");
      return FALSE;
    }
  cairo_set_matrix (cr, &m);
  
  return TRUE;
}

static SeedValue 
seed_cairo_identity_matrix (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 gsize argument_count,
		 const SeedValue arguments[],
		 SeedException *exception)
{
  CHECK_THIS();
  cairo_t *cr = seed_object_get_private (this_object);
  
  cairo_identity_matrix(cr);
  return seed_make_undefined (ctx);
}

static SeedValue
seed_cairo_user_to_device (SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   gsize argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
  SeedValue out[2];
  cairo_t *cr;
  double ix, iy;
  
  CHECK_THIS();
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("user_to_device", "2 arguments");
    }
  cr = seed_object_get_private (this_object);
  ix = seed_value_to_double (ctx, arguments[0], exception);
  iy = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_user_to_device (cr, &ix, &iy);
  out[0] = seed_value_from_double (ctx, ix, exception);
  out[1] = seed_value_from_double (ctx, iy, exception);
  
  return seed_make_array (ctx, out, 2, exception);
}


static SeedValue
seed_cairo_user_to_device_distance (SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   gsize argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
  SeedValue out[2];
  cairo_t *cr;
  double ix, iy;
  
  CHECK_THIS();
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("user_to_device_distance", "2 arguments");
    }
  cr = seed_object_get_private (this_object);
  ix = seed_value_to_double (ctx, arguments[0], exception);
  iy = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_user_to_device_distance (cr, &ix, &iy);
  out[0] = seed_value_from_double (ctx, ix, exception);
  out[1] = seed_value_from_double (ctx, iy, exception);
  
  return seed_make_array (ctx, out, 2, exception);
}

static SeedValue
seed_cairo_device_to_user (SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   gsize argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
  SeedValue out[2];
  cairo_t *cr;
  double ix, iy;
  
  CHECK_THIS();
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("device_to_user", "2 arguments");
    }
  cr = seed_object_get_private (this_object);
  ix = seed_value_to_double (ctx, arguments[0], exception);
  iy = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_device_to_user (cr, &ix, &iy);
  out[0] = seed_value_from_double (ctx, ix, exception);
  out[1] = seed_value_from_double (ctx, iy, exception);
  
  return seed_make_array (ctx, out, 2, exception);
}

static SeedValue
seed_cairo_device_to_user_distance (SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   gsize argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
  SeedValue out[2];
  cairo_t *cr;
  double ix, iy;
  
  CHECK_THIS();
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("device_to_user_distance", "2 arguments");
    }
  cr = seed_object_get_private (this_object);
  ix = seed_value_to_double (ctx, arguments[0], exception);
  iy = seed_value_to_double (ctx, arguments[1], exception);
  
  cairo_device_to_user_distance (cr, &ix, &iy);
  out[0] = seed_value_from_double (ctx, ix, exception);
  out[1] = seed_value_from_double (ctx, iy, exception);
  
  return seed_make_array (ctx, out, 2, exception);
}

seed_static_value cairo_values[] = {
  {"antialias", seed_cairo_get_antialias, seed_cairo_set_antialias, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"fill_rule", seed_cairo_get_fill_rule, seed_cairo_set_fill_rule, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"line_cap", seed_cairo_get_line_cap, seed_cairo_set_line_cap, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"line_join", seed_cairo_get_line_join, seed_cairo_set_line_join, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"line_width", seed_cairo_get_line_width, seed_cairo_set_line_width, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"miter_limit", seed_cairo_get_miter_limit, seed_cairo_set_miter_limit, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"operator", seed_cairo_get_operator, seed_cairo_set_operator, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"tolerance", seed_cairo_get_tolerance, seed_cairo_set_tolerance, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"matrix", seed_cairo_get_matrix, seed_cairo_set_matrix, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"target", seed_cairo_get_target, 0, SEED_PROPERTY_ATTRIBUTE_READ_ONLY | SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {0, 0, 0, 0}
};
  
seed_static_function cairo_funcs[] = {
  {"save", seed_cairo_save, 0},
  {"restore", seed_cairo_restore, 0},
  {"push_group", seed_cairo_push_group, 0},
  {"push_group_with_content", seed_cairo_push_group_with_content, 0},
  {"set_dash", seed_cairo_set_dash, 0},
  {"get_dash_count", seed_cairo_get_dash_count, 0},
  {"get_dash", seed_cairo_get_dash, 0},  
  //  {"pop_group", seed_cairo_pop_group, 0},
  {"pop_group_to_source", seed_cairo_pop_group_to_source, 0},
  {"get_group_target", seed_cairo_get_group_target, 0},
  {"set_source_rgb", seed_cairo_set_source_rgb, 0},
  {"set_source_rgba", seed_cairo_set_source_rgba, 0},
  //  {"set_source", seed_cairo_set_source, 0},
  {"set_source_surface", seed_cairo_set_source_surface, 0},
  //  {"get_source", seed_cairo_get_source, 0},
  {"clip", seed_cairo_clip, 0},
  {"clip_preserve", seed_cairo_clip_preserve, 0},
  {"reset_clip", seed_cairo_reset_clip, 0},
  {"clip_extents", seed_cairo_clip_extents, 0},
  // Rectangle list stuff?
  {"fill", seed_cairo_fill, 0},
  {"fill_preserve", seed_cairo_fill_preserve, 0},
  {"fill_extents", seed_cairo_fill_extents, 0},
  //{"mask", seed_cairo_mask, 0},
  {"in_fill", seed_cairo_in_fill, 0},
  {"mask_surface", seed_cairo_mask_surface, 0},
  {"paint", seed_cairo_paint, 0},
  {"paint_with_alpha", seed_cairo_paint_with_alpha, 0},
  {"stroke", seed_cairo_stroke, 0},
  {"stroke_preserve", seed_cairo_stroke_preserve, 0},
  {"stroke_extents", seed_cairo_stroke_extents, 0},
  {"in_stroke", seed_cairo_in_stroke, 0},
  {"copy_page", seed_cairo_copy_page, 0},
  {"show_page", seed_cairo_show_page, 0},
  {"has_current_point", seed_cairo_has_current_point, 0},
  {"get_current_point", seed_cairo_get_current_point, 0},
  {"new_path", seed_cairo_new_path, 0},
  {"new_sub_path", seed_cairo_new_sub_path, 0},
  {"close_path", seed_cairo_close_path, 0},
  {"arc", seed_cairo_arc, 0},
  {"arc_negative", seed_cairo_arc_negative, 0},
  {"curve_to", seed_cairo_curve_to, 0},
  {"line_to", seed_cairo_line_to, 0},
  {"move_to", seed_cairo_move_to, 0},
  {"rectangle", seed_cairo_rectangle, 0},
  //{"cairo_glpyh_path", seed_cairo_glyph_path, 0},
  {"text_path", seed_cairo_text_path, 0},
  {"rel_curve_to", seed_cairo_rel_curve_to, 0},
  {"rel_line_to", seed_cairo_rel_line_to, 0},
  {"rel_move_to", seed_cairo_rel_move_to, 0},
  {"path_extents", seed_cairo_path_extents, 0},
  {"translate", seed_cairo_translate, 0},
  {"scale", seed_cairo_scale, 0},
  {"rotate", seed_cairo_rotate, 0},
  {"transform", seed_cairo_transform, 0},
  {"identify_matrix", seed_cairo_identity_matrix, 0},
  {"user_to_device", seed_cairo_user_to_device, 0},
  {"user_to_device_distance", seed_cairo_user_to_device_distance, 0},
  {"device_to_user", seed_cairo_device_to_user, 0},
  {"device_to_user_distance", seed_cairo_device_to_user_distance, 0},
  {0, 0, 0}
};

SeedObject
seed_module_init(SeedEngine * local_eng)
{
  SeedObject context_constructor_ref;
  SeedObject namespace_ref;
  seed_class_definition cairo_def = seed_empty_class;
  eng = local_eng;
  namespace_ref = seed_make_object (eng->context, NULL, NULL);
  
  // Temporary hack until API changes.
  seed_value_protect (eng->context, namespace_ref);

  seed_define_cairo_enums (eng->context, namespace_ref);
  seed_define_cairo_surface (eng->context, namespace_ref);
  seed_define_cairo_matrix (eng->context, namespace_ref);
  seed_define_cairo_pattern (eng->context, namespace_ref);
  
  cairo_def.class_name = "CairoContext";
  cairo_def.static_functions = cairo_funcs;
  cairo_def.static_values = cairo_values;
  cairo_def.finalize = seed_cairo_context_finalize;
  seed_cairo_context_class = seed_create_class (&cairo_def);
  // Hack around WebKit GC bug.
  context_constructor_ref = seed_make_constructor (eng->context,
						   NULL,
						   //				   seed_cairo_context_class,
						   seed_cairo_construct_context);

  seed_object_set_property (eng->context, namespace_ref, "Context", context_constructor_ref);
  
  return namespace_ref;
}
