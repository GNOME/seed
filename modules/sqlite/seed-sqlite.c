#include "../../libseed/seed.h"
#include <sqlite3.h>

SeedObject namespace_ref;
SeedClass sqlite_class;

#define MAKE_ERROR_ENUM(name)											\
	seed_object_set_property(eng->context, namespace_ref, #name,			\
							 seed_value_from_int(eng->context, SQLITE_##name, 0))

void define_errors(SeedEngine * eng)
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

void sqlite_database_finalize(SeedObject object)
{
	sqlite3 * db = seed_object_get_private(object);
	if (db)
		sqlite3_close(db);
}

SeedObject sqlite_construct_database(SeedContext ctx,
									  SeedObject constructor,
									  size_t argument_count,
									  const SeedValue arguments[],
									  SeedException * exception)
{
	SeedObject ret;
	gchar * file;
	sqlite3 * db;
	int rc;

	if (argument_count != 1)
	{
		seed_make_exception(ctx, exception, "ArgumentError",
							"sqlite.Database constructor expected 1 argument");
		return (SeedObject)seed_make_null(ctx);
	}
	file = seed_value_to_string(ctx, arguments[0], exception);

	rc = sqlite3_open(file, &db);
	
	ret = seed_make_object(ctx, sqlite_class, db);
	seed_object_set_property(ctx, ret, "status",
							 seed_value_from_int(ctx, rc, exception));

	return ret;
}

SeedValue seed_sqlite_close  (SeedContext ctx,
							  SeedObject function,
							  SeedObject this_object,
							  size_t argument_count,
							  const SeedValue arguments[],
							  SeedException * exception)
{
	sqlite3 * db = seed_object_get_private(this_object);
	sqlite3_close(db);
	return seed_value_from_boolean(ctx, TRUE, exception);
}

seed_static_function database_funcs[] = {
	{"close", seed_sqlite_close, 0},
	{0, 0, 0}
};

void seed_module_init(SeedEngine * eng)
{
	SeedObject db_constructor;
	seed_class_definition sqlite_class_def = seed_empty_class;

	namespace_ref = seed_make_object(eng->context, 0, 0);
	
	seed_object_set_property(eng->context, 
							 eng->global, "sqlite", namespace_ref);
	
	sqlite_class_def.class_name = "Database";
	sqlite_class_def.finalize = sqlite_database_finalize;
	sqlite_class_def.static_functions = database_funcs;

	sqlite_class = seed_create_class(&sqlite_class_def);
	
	db_constructor = seed_make_constructor(eng->context, 
										   sqlite_class,
										   sqlite_construct_database);
	seed_object_set_property(eng->context,
							 namespace_ref, "Database", db_constructor);
}
