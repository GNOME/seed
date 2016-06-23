/*
 * Copyright (c) 2015 Collabora Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <config.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <glib.h>
#include <seed-module.h>
#include <config.h>

#define GJS_COMPAT_VERSION (1 * 100 + 40) * 100 + 0

#define NUMARG_EXPECTED_EXCEPTION(name, argc)                                  \
    seed_make_exception(ctx, exception, "ArgumentError",                       \
                        name " expected " argc " but got %zd", argumentCount); \
    return seed_make_undefined(ctx);

static SeedValue
gjs_override_property(SeedContext ctx,
                      SeedObject function,
                      SeedObject this_object,
                      size_t argumentCount,
                      const SeedValue arguments[],
                      SeedException* exception)
{
    // TODO: to be implemented.
    seed_make_exception(ctx, exception, "ImplementationError",
                        "Not implemented yet");
    return seed_make_undefined(ctx);
}

static SeedValue
gjs_register_interface(SeedContext ctx,
                       SeedObject function,
                       SeedObject this_object,
                       size_t argumentCount,
                       const SeedValue arguments[],
                       SeedException* exception)
{
    // TODO: to be implemented.
    seed_make_exception(ctx, exception, "ImplementationError",
                        "Not implemented yet");
    return seed_make_undefined(ctx);
}
static SeedValue
gjs_register_type(SeedContext ctx,
                  SeedObject function,
                  SeedObject this_object,
                  size_t argumentCount,
                  const SeedValue arguments[],
                  SeedException* exception)
{
    // TODO: to be implemented.
    seed_make_exception(ctx, exception, "ImplementationError",
                        "Not implemented yet");
    return seed_make_undefined(ctx);
}

static SeedValue
gjs_add_interface(SeedContext ctx,
                  SeedObject function,
                  SeedObject this_object,
                  size_t argumentCount,
                  const SeedValue arguments[],
                  SeedException* exception)
{
    // TODO: to be implemented.
    seed_make_exception(ctx, exception, "ImplementationError",
                        "Not implemented yet");
    return seed_make_undefined(ctx);
}

static SeedValue
gjs_hook_up_vfunc(SeedContext ctx,
                  SeedObject function,
                  SeedObject this_object,
                  size_t argumentCount,
                  const SeedValue arguments[],
                  SeedException* exception)
{
    // TODO: to be implemented.
    seed_make_exception(ctx, exception, "ImplementationError",
                        "Not implemented yet");
    return seed_make_undefined(ctx);
}

static SeedValue
gjs_signal_new(SeedContext ctx,
               SeedObject function,
               SeedObject this_object,
               size_t argumentCount,
               const SeedValue arguments[],
               SeedException* exception)
{
    // TODO: to be implemented.
    seed_make_exception(ctx, exception, "ImplementationError",
                        "Not implemented yet");
    return seed_make_undefined(ctx);
}

static seed_static_function module_funcs[]
  = { { "override_property", gjs_override_property, 0 },
      { "register_interface", gjs_register_interface, 0 },
      { "register_type", gjs_register_type, 0 },
      { "add_interface", gjs_add_interface, 0 },
      { "hook_up_vfunc", gjs_hook_up_vfunc, 0 },
      { "signal_new", gjs_signal_new, 0 },
      { 0, 0, 0 } };

static seed_class_definition system_def = {
    0,                                        /* Version, always 0 */
    SEED_CLASS_ATTRIBUTE_NO_SHARED_PROTOTYPE, /* JSClassAttributes */
    "_gi",                                    /* Class Name */
    NULL,                                     /* Parent Class */
    NULL,                                     /* Static Values */
    module_funcs,                             /* Static Functions */
    NULL,
    NULL, /* Finalize */
    NULL, /* Has Property */
    NULL, /* Get Property */
    NULL, /* Set Property */
    NULL, /* Delete Property */
    NULL, /* Get Property Names */
    NULL, /* Call As Function */
    NULL, /* Call As Constructor */
    NULL, /* Has Instance */
    NULL  /* Convert To Type */
};

SeedObject
gjs_compat_define_gi_stuff(SeedEngine* eng)
{
    SeedContext context = eng->context;
    SeedObject module;
    gboolean ret;

    module = seed_make_object(context, seed_create_class(&system_def), NULL);

    SeedValue seed = seed_object_get_property(context, eng->global, "Seed");
    SeedValue argv = seed_object_get_property(context, seed, "argv");

    ret = seed_object_set_property(
      context, module, "version",
      (SeedValue) seed_value_from_int(context, GJS_COMPAT_VERSION, NULL));

    return module;
}

SeedObject
seed_module_init(SeedEngine* eng)
{
    return gjs_compat_define_gi_stuff(eng);
}
