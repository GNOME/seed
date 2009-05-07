#include <seed.h>

SeedContext ctx;
SeedContextGroup group;
SeedObject namespace_ref;
SeedClass context_class;

static SeedObject
seed_construct_sandbox_context (SeedContext ctx,
				SeedObject constructor,
				size_t argument_count,
				const SeedValue arguments[],
				SeedException * exception)
{
  SeedObject ret;
  SeedContext c;
  
  c = seed_context_create (group, NULL);
  ret = seed_make_object (ctx, context_class, c);
  
  seed_object_set_property (ctx, ret, "global", seed_context_get_global_object (c));
  return ret;
}

static SeedValue
seed_context_eval (SeedContext ctx,
		  SeedObject function,
		  SeedObject this_object,
		  size_t argument_count,
		  const SeedValue arguments[], 
		  SeedException * exception)
{
  SeedContext c = seed_object_get_private (this_object);
  SeedValue ret;
  gchar *s;
  
  s = seed_value_to_string (ctx, arguments[0], exception);
  ret = seed_simple_evaluate (c, s, exception);
  g_free (s);
  
  return ret;
}

static SeedValue
seed_sandbox_context_add_globals (SeedContext ctx,
				  SeedObject function,
				  SeedObject this_object,
				  size_t argument_count,
				  const SeedValue arguments[], 
				  SeedException * exception)
{
  SeedContext c = seed_object_get_private (this_object);
  seed_prepare_global_context (c);
  
  return seed_make_null (ctx);
}

seed_static_function context_funcs[] = {
  {"eval", seed_context_eval, 0},
  {"add_globals", seed_sandbox_context_add_globals, 0},
  {0, 0, 0}
};

static void
context_finalize (SeedObject object)
{
  SeedContext *c = seed_object_get_private (object);
  
  seed_context_unref (c);
}

SeedObject
seed_module_init(SeedEngine * eng)
{
  SeedObject context_constructor;
  seed_class_definition context_class_def = seed_empty_class;

  ctx = eng->context;
  group = eng->group;
  namespace_ref = seed_make_object (ctx, NULL, NULL);
  
  context_class_def.class_name = "Context";
  context_class_def.static_functions = context_funcs;
  context_class_def.finalize = context_finalize;
  
  context_class = seed_create_class (&context_class_def);
  
  
  context_constructor = seed_make_constructor (eng->context,
					       context_class,
					       seed_construct_sandbox_context);
  seed_object_set_property (eng->context, namespace_ref, "Context", context_constructor);
  
  return namespace_ref;

}
