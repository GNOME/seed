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
 * Copyright © 2016 Robert Bosch Car Multimedia GmbH
 */

#include "../../libseed/seed.h"

static void
test_module_whitelist(void)
{
    SeedEngine* engine;
    SeedValue* val;
    SeedException exception = NULL;

    engine = seed_init_constrained(NULL, NULL);

    /* In the initial state, the engine is completely sandboxed
     */
    val = seed_simple_evaluate(engine->context, "Os = imports.os;", &exception);
    g_assert(val == NULL);
    g_assert_cmpstr(seed_exception_get_message(engine->context, exception), ==,
                    "Can't find variable: imports");
    exception = NULL;

    /* Even printing is impossible */
    val = seed_simple_evaluate(engine->context, "print (\"Hello world!\");",
                               &exception);
    g_assert(val == NULL);
    g_assert_cmpstr(seed_exception_get_message(engine->context, exception), ==,
                    "Can't find variable: print");
    exception = NULL;

    /* Let's fix that */
    seed_init_builtins(engine, NULL, NULL);
    val = seed_simple_evaluate(engine->context, "print (\"Hello world!\");",
                               &exception);
    g_assert(val != NULL);
    g_assert(exception == NULL);

    /* imports is not a builtin, and thus should not yet be available */
    val = seed_simple_evaluate(engine->context, "Os = imports.os;", &exception);
    g_assert(val == NULL);
    g_assert_cmpstr(seed_exception_get_message(engine->context, exception), ==,
                    "Can't find variable: imports");
    exception = NULL;

    /* Let's fix that */
    seed_engine_initialize_importer(engine);
    val = seed_simple_evaluate(engine->context, "Os = imports.os;", &exception);
    g_assert(exception == NULL);
    g_assert(val != NULL);

    /* As imports is now available, seed doesn't raise an exception anymore,
     * however as the search path for modules is currently empty, the type of
     * val should be undefined.
     */
    g_assert(seed_value_get_type(engine->context, val) == SEED_TYPE_UNDEFINED);

    /* Let's update the search path, this is set in Makefile.am
     * to an uninstalled location which is guaranteed to contain an
     * os module
     */
    {
        const gchar* env_paths = g_getenv("SEED_MODULE_PATH");
        g_assert(env_paths != NULL);
        gchar** paths = g_strsplit(env_paths, ":", -1);
        seed_importer_set_search_path(engine->context, paths);
        g_strfreev(paths);
    }

    val = seed_simple_evaluate(engine->context, "Os = imports.os;", &exception);
    g_assert(exception == NULL);
    g_assert(val != NULL);
    /* imports.os is no longer undefined, but an actual object */
    g_assert(seed_value_get_type(engine->context, val) == SEED_TYPE_OBJECT);

    /* We're now less sandboxed, but there are still some things we can't do,
     * for example importing any gi module */
    val = seed_simple_evaluate(engine->context, "GObject = imports.gi.GObject;",
                               &exception);
    g_assert(val == NULL);

    /* Pretty obscure error message, but the idea is there */
    g_assert_cmpstr(
      seed_exception_get_message(engine->context, exception), ==,
      "undefined is not an object (evaluating 'imports.extensions.GObject')");
    exception = NULL;

    /* Let's now authorize GObject and GObject only */
    val = seed_engine_expose_namespace(engine, "GObject", exception);
    g_assert(val != NULL);
    g_assert(exception == NULL);

    /* Seed tells us all went well, let's try anyway */
    val = seed_simple_evaluate(engine->context, "GObject = imports.gi.GObject;",
                               &exception);
    g_assert(val != NULL);
    g_assert(exception == NULL);

    /* Only need to check whether another module which we know
     * would be available if the engine was not constrained is indeed
     * still unavailable
     */
    val = seed_simple_evaluate(engine->context, "GLib = imports.gi.GLib;",
                               &exception);
    g_assert(val == NULL);

    /* The error message isn't any clearer since last time */
    g_assert_cmpstr(
      seed_exception_get_message(engine->context, exception), ==,
      "undefined is not an object (evaluating 'imports.extensions.GLib')");
    exception = NULL;

    seed_engine_destroy(engine);
}

int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/module-whitelist", test_module_whitelist);
    return g_test_run();
}
