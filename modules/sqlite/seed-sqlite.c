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

#include "../../libseed/seed.h"
#include <sqlite3.h>

SeedObject namespace_ref;
SeedClass sqlite_class;
SeedEngine* eng;

#define MAKE_ERROR_ENUM(name)                                                  \
    seed_object_set_property(eng->context, namespace_ref, #name,               \
                             seed_value_from_int(eng->context, SQLITE_##name,  \
                                                 0))

void
define_errors(SeedEngine* eng)
{
    MAKE_ERROR_ENUM(OK);
    MAKE_ERROR_ENUM(ERROR);
    MAKE_ERROR_ENUM(INTERNAL);
    MAKE_ERROR_ENUM(PERM);
    MAKE_ERROR_ENUM(ABORT);
    MAKE_ERROR_ENUM(BUSY);
    MAKE_ERROR_ENUM(LOCKED);
    MAKE_ERROR_ENUM(NOMEM);
    MAKE_ERROR_ENUM(READONLY);
    MAKE_ERROR_ENUM(INTERRUPT);
    MAKE_ERROR_ENUM(CORRUPT);
    MAKE_ERROR_ENUM(NOTFOUND);
    MAKE_ERROR_ENUM(FULL);
    MAKE_ERROR_ENUM(CANTOPEN);
    MAKE_ERROR_ENUM(PROTOCOL);
    MAKE_ERROR_ENUM(EMPTY);
    MAKE_ERROR_ENUM(SCHEMA);
    MAKE_ERROR_ENUM(TOOBIG);
    MAKE_ERROR_ENUM(CONSTRAINT);
    MAKE_ERROR_ENUM(MISMATCH);
    MAKE_ERROR_ENUM(MISUSE);
    MAKE_ERROR_ENUM(NOLFS);
    MAKE_ERROR_ENUM(AUTH);
    MAKE_ERROR_ENUM(FORMAT);
    MAKE_ERROR_ENUM(RANGE);
    MAKE_ERROR_ENUM(NOTADB);
    MAKE_ERROR_ENUM(ROW);
    MAKE_ERROR_ENUM(DONE);
}

void
sqlite_database_finalize(SeedObject object)
{
    sqlite3* db = seed_object_get_private(object);
    if (db)
        sqlite3_close(db);
}

SeedObject
sqlite_construct_database(SeedContext ctx,
                          SeedObject constructor,
                          size_t argument_count,
                          const SeedValue arguments[],
                          SeedException* exception)
{
    SeedObject ret;
    gchar* file;
    sqlite3* db;
    int rc;

    if (argument_count != 1) {
        seed_make_exception(ctx, exception, "ArgumentError",
                            "sqlite.Database constructor expected 1 argument");
        return (SeedObject) seed_make_null(ctx);
    }
    file = seed_value_to_string(ctx, arguments[0], exception);

    rc = sqlite3_open(file, &db);

    g_free(file);

    ret = seed_make_object(ctx, sqlite_class, db);
    seed_object_set_property(ctx, ret, "status",
                             seed_value_from_int(ctx, rc, exception));

    return ret;
}

static int
seed_sqlite_exec_callback(SeedObject function,
                          int argc,
                          gchar** argv,
                          gchar** azColName)
{
    SeedGlobalContext ctx;
    SeedObject hash;
    int i;

    if (!function)
        return 0;

    ctx = seed_context_create(eng->group, NULL);

    hash = seed_make_object(ctx, 0, 0);
    for (i = 0; i < argc; i++) {
        seed_object_set_property(ctx, hash, azColName[i],
                                 seed_value_from_string(ctx, argv[i], 0));
    }

    seed_object_call(ctx, function, 0, 1, &hash, 0);

    seed_context_unref(ctx);

    return 0;
}

SeedValue
seed_sqlite_exec(SeedContext ctx,
                 SeedObject function,
                 SeedObject this_object,
                 size_t argument_count,
                 const SeedValue arguments[],
                 SeedException* exception)
{
    gchar* statement;
    gchar* sqlite_error = 0;
    sqlite3* db;
    int rc;

    if (argument_count < 1) {
        seed_make_exception(ctx, exception, "ArgumentError",
                            "sqlite.Database.exec expected 1 or 2 arguments");
        return seed_make_null(ctx);
    }

    statement = seed_value_to_string(ctx, arguments[0], exception);
    db = seed_object_get_private(this_object);

    g_assert(db);

    rc = sqlite3_exec(db, statement, seed_sqlite_exec_callback,
                      argument_count == 2 ? arguments[1] : 0, &sqlite_error);
    g_free(statement);

    if (rc != SQLITE_OK) {
        if (sqlite_error) {
            seed_make_exception(ctx, exception, "SqliteError", sqlite_error,
                                NULL);
            sqlite3_free(sqlite_error);
        }
        return seed_make_null(ctx);
    }

    return seed_value_from_int(ctx, rc, exception);
}

SeedValue
seed_sqlite_close(SeedContext ctx,
                  SeedObject function,
                  SeedObject this_object,
                  size_t argument_count,
                  const SeedValue arguments[],
                  SeedException* exception)
{
    sqlite3* db = seed_object_get_private(this_object);
    sqlite3_close(db);
    return seed_value_from_boolean(ctx, TRUE, exception);
}

seed_static_function database_funcs[] = { { "close", seed_sqlite_close, 0 },
                                          { "exec", seed_sqlite_exec, 0 },
                                          { NULL, NULL, 0 } };

SeedObject
seed_module_init(SeedEngine* local_eng)
{
    SeedObject db_constructor;
    seed_class_definition sqlite_class_def = seed_empty_class;

    eng = local_eng;

    namespace_ref = seed_make_object(eng->context, 0, 0);

    define_errors(eng);

    sqlite_class_def.class_name = "Database";
    sqlite_class_def.finalize = sqlite_database_finalize;
    sqlite_class_def.static_functions = database_funcs;

    sqlite_class = seed_create_class(&sqlite_class_def);

    db_constructor = seed_make_constructor(eng->context, sqlite_class,
                                           sqlite_construct_database);
    seed_object_set_property(eng->context, namespace_ref, "Database",
                             db_constructor);

    return namespace_ref;
}
