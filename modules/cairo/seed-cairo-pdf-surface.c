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
#include <cairo/cairo-pdf.h>

#include "seed-cairo.h"
#include "seed-cairo-surface.h"

#define CAIRO_SURFACE_PRIV(obj)                                                \
    ((cairo_surface_t*) seed_object_get_private(obj))

#define CHECK_SURFACE(obj)                                                     \
    ({                                                                         \
        if (!seed_object_is_of_class(ctx, obj,                                 \
                                     seed_cairo_pdf_surface_class)) {          \
            seed_make_exception(ctx, exception, "ArgumentError",               \
                                "Object is not a Cairo Surface");              \
            return seed_make_undefined(ctx);                                   \
        }                                                                      \
        if (!seed_object_get_private(obj)) {                                   \
            seed_make_exception(ctx, exception, "ArgumentError",               \
                                "Cairo surface has been destroyed");           \
            return seed_make_undefined(ctx);                                   \
        }                                                                      \
    })

#define CHECK_THIS()                                                           \
    if (!seed_object_get_private(this_object)) {                               \
        seed_make_exception(ctx, exception, "ArgumentError",                   \
                            "Cairo surface has been destroyed");               \
        return seed_make_undefined(ctx);                                       \
    }

SeedClass seed_cairo_pdf_surface_class;
SeedObject pdf_surface_constructor_ref;

static SeedObject
seed_object_from_cairo_pdf_surface(SeedContext ctx, cairo_surface_t* surf)
{
    SeedObject jsobj;

    jsobj = cairo_surface_get_user_data(surf, seed_get_cairo_key());
    if (jsobj)
        return jsobj;

    jsobj = seed_make_object(ctx, seed_cairo_pdf_surface_class, surf);
    cairo_surface_set_user_data(surf, seed_get_cairo_key(), jsobj,
                                seed_cairo_destroy_func);
    return jsobj;
}

static SeedValue
seed_cairo_pdf_surface_set_size(SeedContext ctx,
                                SeedObject function,
                                SeedObject this_object,
                                gsize argument_count,
                                const SeedValue arguments[],
                                SeedException* exception)
{
    cairo_surface_t* surf;
    gdouble x, y;

    CHECK_THIS();
    if (argument_count != 2) {
        EXPECTED_EXCEPTION("set_size", "2 arguments");
    }
    surf = seed_object_get_private(this_object);
    x = seed_value_to_double(ctx, arguments[0], exception);
    y = seed_value_to_double(ctx, arguments[1], exception);

    cairo_pdf_surface_set_size(surf, x, y);

    return seed_make_undefined(ctx);
}

static SeedObject
seed_cairo_construct_pdf_surface(SeedContext ctx,
                                 SeedObject constructor,
                                 size_t argument_count,
                                 const SeedValue arguments[],
                                 SeedException* exception)
{
    cairo_surface_t* ret;
    gchar* filename = NULL;
    gdouble width, height;
    if (argument_count != 3) {
        EXPECTED_EXCEPTION("PDFSurface", "3 arguments");
    }

    if (!seed_value_is_null(ctx, arguments[0]))
        filename = seed_value_to_string(ctx, arguments[0], exception);
    width = seed_value_to_double(ctx, arguments[1], exception);
    height = seed_value_to_double(ctx, arguments[2], exception);
    ret = cairo_pdf_surface_create(filename, width, height);

    return seed_object_from_cairo_pdf_surface(ctx, ret);
}

seed_static_value pdf_surface_values[] = { { 0, 0, 0, 0 } };

seed_static_function pdf_surface_funcs[]
  = { { "set_size", seed_cairo_pdf_surface_set_size, 0 }, { 0, 0, 0 } };

void
seed_define_cairo_pdf_surface(SeedContext ctx, SeedObject namespace_ref)
{
    seed_class_definition pdf_def = seed_empty_class;

    pdf_def.class_name = "PDFSurface";
    pdf_def.static_values = pdf_surface_values;
    pdf_def.parent_class = seed_get_cairo_surface_class();
    pdf_def.static_functions = pdf_surface_funcs;
    seed_cairo_pdf_surface_class = seed_create_class(&pdf_def);

    pdf_surface_constructor_ref
      = seed_make_constructor(ctx, NULL,
                              // seed_cairo_pdf_surface_class,
                              seed_cairo_construct_pdf_surface);
    seed_object_set_property(ctx, namespace_ref, "PDFSurface",
                             pdf_surface_constructor_ref);
}
