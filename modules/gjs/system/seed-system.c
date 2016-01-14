/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
 * Copyright (c) 2008  litl, LLC
 * Copyright (c) 2012  Red Hat, Inc.
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

#define NUMARG_EXPECTED_EXCEPTION(name, argc) \
    seed_make_exception (ctx, exception, "ArgumentError", name " expected " argc  " but got %zd", argumentCount); \
    return seed_make_undefined(ctx);

static SeedValue
gjs_address_of (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			size_t argumentCount,
			const SeedValue arguments[], SeedException * exception)
{
    if (argumentCount != 1) {
        NUMARG_EXPECTED_EXCEPTION("addressOf", "1 argument")
    }

    SeedValue targetValue = arguments[0];
    if (!seed_value_is_object(ctx, targetValue)) {
        seed_make_exception (ctx, exception, "ArgumentError", "addressOf expects an object");
        return seed_make_undefined(ctx);
    }

    char *pointer_string = g_strdup_printf("%p", targetValue);
    SeedValue ret = seed_value_from_string(ctx, pointer_string, exception);
    g_free(pointer_string);

    return ret;
}

static SeedValue
gjs_refcount (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			size_t argumentCount,
			const SeedValue arguments[], SeedException * exception)
{
    if (argumentCount != 1) {
         NUMARG_EXPECTED_EXCEPTION("refcount", "1 argument");
    }

    SeedValue targetValue = arguments[0];
    GObject *object = seed_value_to_object (ctx, targetValue, exception);
    if (!object)
        return seed_make_undefined(ctx);

    SeedValue ret = seed_value_from_uint64(ctx, object->ref_count, exception);
    return ret;
}

static SeedValue
gjs_breakpoint (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			size_t argumentCount,
			const SeedValue arguments[], SeedException * exception)
{
    if (argumentCount != 0) {
         NUMARG_EXPECTED_EXCEPTION("breakpoint", "0 arguments");
    }

    G_BREAKPOINT();
	return seed_make_undefined (ctx);
}

static SeedValue
gjs_gc (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			size_t argumentCount,
			const SeedValue arguments[], SeedException * exception)
{
    if (argumentCount != 0) {
         NUMARG_EXPECTED_EXCEPTION("gc", "0 arguments");
    }

    seed_context_collect(ctx);
	return seed_make_undefined (ctx);
}

static SeedValue
gjs_exit (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			size_t argumentCount,
			const SeedValue arguments[], SeedException * exception)
{
    gint32 ret = EXIT_SUCCESS;
    if (argumentCount > 1) {
        NUMARG_EXPECTED_EXCEPTION("exit", "none or 1 argument");
    }

    if (argumentCount == 1) {
        SeedValue target = arguments[0];
        if (seed_value_is_number(ctx, target)) {
            ret = seed_value_to_int(ctx, target, exception);
        } else {
            seed_make_exception (ctx, exception, "ArgumentError", "exit expects a number argument");
            ret = EXIT_FAILURE;
        }
    }

    exit(ret);
}

static SeedValue
gjs_clear_date_caches (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			size_t argumentCount,
			const SeedValue arguments[], SeedException * exception)
{
/*static SeedValue
gjs_clear_date_caches(SeedContext context,
             unsigned   argc,
             SeedValue      *vp)
{
    JS::CallReceiver rec = JS::CallReceiverFromVp(vp);
    JS_BeginRequest(context);

    // Workaround for a bug in SpiderMonkey where tzset is not called before
    // localtime_r, see https://bugzilla.mozilla.org/show_bug.cgi?id=1004706
    tzset();

    JS_ClearDateCaches(context);
    JS_EndRequest(context);

    rec.rval().set(JSVAL_VOID);
    return TRUE;*/
	return seed_value_from_boolean (ctx, TRUE, exception);
}

static seed_static_function module_funcs[] = {
    {"addressOf", gjs_address_of, 0},
    {"refcount", gjs_refcount, 0},
    {"breakpoint", gjs_breakpoint, 0},
    {"gc", gjs_gc, 0},
    {"exit", gjs_exit, 0},
    {"clearDateCaches", gjs_clear_date_caches, 0},
  {0, 0, 0}
};

static seed_class_definition system_def = {
  0,				/* Version, always 0 */
  SEED_CLASS_ATTRIBUTE_NO_SHARED_PROTOTYPE,	/* JSClassAttributes */
  "System",			/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  module_funcs,		/* Static Functions */
  NULL,
  NULL,	/* Finalize */
  NULL,				/* Has Property */
  NULL,	/* Get Property */
  NULL,	/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL  /* Convert To Type */
};

SeedObject
gjs_compat_define_system_stuff(SeedEngine *eng)
{
	SeedContext context = eng->context;
    char *program_name = "gnome-weather";
    SeedObject module;
    gboolean ret;

    module = seed_make_object(context, seed_create_class(&system_def), NULL);
    g_print("Module: %p\n", module);

    SeedValue seed = seed_object_get_property(context, eng->global, "Seed");
    SeedValue argv = seed_object_get_property(context, seed, "argv");

    g_print("Seed: %p\n", seed);
    g_print("Argv: %p\n", argv);

    //gjs_context = (GjsContext*) JS_GetContextPrivate(context);
    //g_object_get(gjs_context,
    //             "program-name", &program_name,
    //             NULL);

    //if (!gjs_string_from_utf8(context, program_name,
    //                          -1, &value))
    //    goto out;

    /* The name is modeled after program_invocation_name,
       part of the glibc */
    ret = seed_object_set_property (context, module, "programInvocationName",
    		(SeedValue)seed_value_from_string (context, program_name, NULL));
    g_print("Module: %d\n", ret);

    ret = seed_object_set_property (context, module, "version",
    		(SeedValue)seed_value_from_int (context, GJS_COMPAT_VERSION, NULL));
    g_print("Module: %d\n", ret);

    return module;
}

SeedObject
seed_module_init(SeedEngine * eng)
{
  return  gjs_compat_define_system_stuff(eng);
}
