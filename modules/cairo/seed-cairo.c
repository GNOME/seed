#include <seed.h>
#include <cairo/cairo.h>
#include "seed-cairo.h"
#include "seed-cairo-surface.h"
#include "seed-cairo-image-surface.h"
#include "seed-cairo-enums.h"


SeedEngine *eng;

#define CAIRO_CONTEXT_PRIV(obj) ((cairo_t *)seed_object_get_private(obj))

#define CHECK_CAIRO(obj) ({						\
    if (!seed_object_is_of_class (ctx, obj, seed_cairo_class)){	\
      seed_make_exception (ctx, exception, "ArgumentError", "Object is not a Cairo Context"); \
      return seed_make_undefined (ctx);\
    }									\
    if (!seed_object_get_private (obj)){				\
      seed_make_exception (ctx, exception, "ArgumentError", "Cairo Context has been destroyed"); \
      return seed_make_undefined (ctx);}})

#define CHECK_THIS() if (!seed_object_get_private (this_object)){	\
    seed_make_exception (ctx, exception, "ArgumentError", "Cairo Context has been destroyed"); \
      return seed_make_undefined (ctx);}

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
		 SeedObject function,
		 SeedObject this_object,
		 gsize argument_count,
		 const SeedValue arguments[],
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


seed_static_function cairo_funcs[] = {
  {"save", seed_cairo_save, 0},
  {"restore", seed_cairo_restore, 0},
  {"get_target", seed_cairo_get_target, 0},
  {"push_group", seed_cairo_push_group, 0},
  {"push_group_with_content", seed_cairo_push_group_with_content, 0},
  //  {"pop_group", seed_cairo_pop_group, 0},
  {"pop_group_to_source", seed_cairo_pop_group_to_source, 0},
  {"get_group_target", seed_cairo_get_group_target, 0},
  {0, 0, 0}
};

SeedObject
seed_module_init(SeedEngine * local_eng)
{
  SeedClass context_constructor_class;
  //SeedObject context_constructor_ref;
  SeedObject namespace_ref;
  seed_class_definition context_constructor_class_def = seed_empty_class;
  seed_class_definition cairo_def = seed_empty_class;
  eng = local_eng;
  namespace_ref = seed_make_object (eng->context, NULL, NULL);
  
  // Temporary hack until API changes.
  seed_value_protect (eng->context, namespace_ref);
  seed_define_cairo_enums (eng->context, namespace_ref);
  seed_define_cairo_surface (eng->context, namespace_ref);
  
  cairo_def.class_name = "CairoContext";
  cairo_def.finalize = seed_cairo_context_finalize;
  seed_cairo_context_class = seed_create_class (&cairo_def);

// Hack around WebKit GC bug.m
//  context_constructor_ref = seed_make_constructor (eng->context,
//						   seed_cairo_context_class,
//						   seed_cairo_construct_context);

  context_constructor_class_def.class_name = "CairoContextConstructor";
  context_constructor_class_def.call_as_constructor = seed_cairo_construct_context;
  
  context_constructor_class = seed_create_class(&context_constructor_class_def);
  seed_object_set_property (eng->context, namespace_ref, "Context", seed_make_object (eng->context, context_constructor_class, NULL));

  //  seed_object_set_property (eng->context, namespace_ref, "Context", context_constructor_ref);
  
  return namespace_ref;
}
