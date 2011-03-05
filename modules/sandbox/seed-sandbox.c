/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2 of
 * the License, or (at your option) any later version.
 * Seed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with Seed.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) Robert Carr 2009 <carrr@rpi.edu>
 */

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

  if (!c)
    {
      seed_make_exception (ctx, exception,
			   "ArgumentError", "Context is destroyed");
      return seed_make_undefined (ctx);
    }

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
  if (!c)
    {
      seed_make_exception (ctx, exception,
			   "ArgumentError", "Context is destroyed");
      return seed_make_undefined (ctx);
    }
  seed_prepare_global_context (c);

  SeedObject g = seed_context_get_global_object (c);
  SeedObject global = seed_context_get_global_object (ctx);

  SeedValue script_path = seed_object_get_property (ctx, global, "__script_path__"); 

  seed_object_set_property (c, g, "__script_path__", 
     (script_path && seed_value_is_object(ctx, script_path)) ?
     script_path : seed_make_undefined(ctx)
  );
    
  return seed_make_null (ctx);
}


static SeedValue
seed_sandbox_context_destroy (SeedContext ctx,
			      SeedObject function,
			      SeedObject this_object,
			      size_t argument_count,
			      const SeedValue arguments[],
			      SeedException * exception)
{
  SeedContext c = seed_object_get_private (this_object);

  seed_context_unref (c);
  seed_object_set_private (this_object, NULL);
  return seed_make_null (ctx);
}

seed_static_function context_funcs[] = {
  {"eval", seed_context_eval, 0},
  {"add_globals", seed_sandbox_context_add_globals, 0},
  {"destroy", seed_sandbox_context_destroy, 0},
  {NULL, NULL, 0}
};

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

  context_class = seed_create_class (&context_class_def);


  context_constructor = seed_make_constructor (eng->context,
					       context_class,
					       seed_construct_sandbox_context);
  seed_object_set_property (eng->context, namespace_ref, "Context", context_constructor);

  return namespace_ref;

}
