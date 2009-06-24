#include <seed.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib/gi18n.h>

SeedObject namespace_ref;
SeedEngine *eng;

static SeedValue
seed_gettext_i18n (SeedContext ctx,
                   SeedObject function,
                   SeedObject this_object,
                   gsize argument_count,
                   const SeedValue arguments[],
                   SeedException * exception)
{
	gchar * string_to_translate;

	if (argument_count != 1)
	{
		seed_make_exception (ctx, exception, "ArgumentError", 
		                     "gettext.i18n expected 1 argument, got %zd", 
		                     argument_count);
		return seed_make_null (ctx);
	}
	
	string_to_translate = seed_value_to_string (ctx, arguments[0], exception);
	
	return seed_value_from_string (ctx, _(string_to_translate), exception);  
}

static void
seed_gettext_define_stuff ()
{
	seed_create_function(eng->context, "i18n", 
	                     (SeedFunctionCallback) seed_gettext_i18n,
	                     namespace_ref);
}

SeedObject
seed_module_init(SeedEngine *local_eng)
{
	eng = local_eng;
	namespace_ref = seed_make_object (eng->context, NULL, NULL);
	seed_value_protect (eng->context, namespace_ref);

	seed_gettext_define_stuff();

	return namespace_ref;
}
