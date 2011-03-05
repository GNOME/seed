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
#include <cairo/cairo.h>

#include "seed-cairo.h"
#include "seed-cairo-surface.h"

#define CAIRO_SURFACE_PRIV(obj) ((cairo_surface_t *)seed_object_get_private(obj))

#define CHECK_SURFACE(obj) ({					\
      if (!seed_object_is_of_class (ctx, obj, seed_cairo_image_surface_class)){	\
	seed_make_exception (ctx, exception, "ArgumentError", "Object is not a Cairo Surface"); \
	return seed_make_undefined (ctx);					\
      }									\
      if (!seed_object_get_private (obj)){				\
	seed_make_exception (ctx, exception, "ArgumentError", "Cairo surface has been destroyed"); \
	return seed_make_undefined (ctx);}})

#define CHECK_THIS() if (!seed_object_get_private (this_object)){	\
    seed_make_exception (ctx, exception, "ArgumentError", "Cairo surface has been destroyed"); \
    return seed_make_undefined (ctx);}

SeedClass seed_cairo_image_surface_class;
SeedObject image_surface_constructor_ref;


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
				     SeedObject this_object,
				     SeedString property_name,
				     SeedException *exception)
{
  CHECK_THIS();
  return seed_value_from_long (ctx, cairo_image_surface_get_format (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedValue
seed_cairo_image_surface_get_width (SeedContext ctx,
				    SeedObject this_object,
				    SeedString property_name,
				    SeedException *exception)
{
  CHECK_THIS();
  return seed_value_from_int (ctx, cairo_image_surface_get_width (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedValue
seed_cairo_image_surface_get_height (SeedContext ctx,
				     SeedObject this_object,
				     SeedString property_name,
				     SeedException *exception)
{
  CHECK_THIS();
  return seed_value_from_int (ctx, cairo_image_surface_get_height (seed_object_to_cairo_surface(ctx, this_object, exception)), exception);
}

static SeedValue
seed_cairo_image_surface_get_stride (SeedContext ctx,
				     SeedObject this_object,
				     SeedString property_name,
				     SeedException *exception)
{
  CHECK_THIS();
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

seed_static_value image_surface_values[] = {
  {"format", seed_cairo_image_surface_get_format, 0, SEED_PROPERTY_ATTRIBUTE_READ_ONLY | SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"width", seed_cairo_image_surface_get_width, 0, SEED_PROPERTY_ATTRIBUTE_READ_ONLY | SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"height", seed_cairo_image_surface_get_height, 0, SEED_PROPERTY_ATTRIBUTE_READ_ONLY | SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {"stride", seed_cairo_image_surface_get_stride, 0, SEED_PROPERTY_ATTRIBUTE_READ_ONLY | SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {0, 0, 0, 0}
};




void
seed_define_cairo_image_surface (SeedContext ctx,
				 SeedObject namespace_ref)
{
  seed_class_definition image_def = seed_empty_class;
  // TODO: FIX
  seed_static_function webkit_fail = {0,0,0};

  image_def.class_name = "ImageSurface";
  image_def.static_values = image_surface_values;
  image_def.parent_class = seed_get_cairo_surface_class ();
  // FAIL
  image_def.static_functions = &webkit_fail;
  seed_cairo_image_surface_class = seed_create_class (&image_def);

  image_surface_constructor_ref = seed_make_constructor (ctx,
							 NULL,
							 //seed_cairo_image_surface_class,
							 seed_cairo_construct_image_surface);
  seed_object_set_property (ctx, namespace_ref, "ImageSurface", image_surface_constructor_ref);
}
