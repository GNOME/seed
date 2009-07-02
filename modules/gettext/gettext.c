#include <seed.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib/gi18n.h>

SeedObject namespace_ref;
SeedEngine *eng;

static SeedValue
seed_gettext_gettext (SeedContext ctx,
                      SeedObject function,
                      SeedObject this_object,
                      gsize argument_count,
                      const SeedValue arguments[],
                      SeedException * exception)
{
	gchar * msgid;

	if (argument_count != 1)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.gettext expected 1 argument, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	msgid = seed_value_to_string (ctx, arguments[0], exception);

	return seed_value_from_string (ctx, gettext(msgid), exception);
}

static SeedValue
seed_gettext_textdomain (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue arguments[],
                         SeedException * exception)
{
	gchar * domain_name;

	if (argument_count != 1)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.textdomain expected 1 argument, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	domain_name = seed_value_to_string (ctx, arguments[0], exception);

	return seed_value_from_string (ctx, textdomain(domain_name), exception);
}

static void
seed_gettext_define_stuff ()
{
	seed_create_function(eng->context, "gettext",
	                     (SeedFunctionCallback) seed_gettext_gettext,
	                     namespace_ref);

	seed_create_function(eng->context, "textdomain",
	                     (SeedFunctionCallback) seed_gettext_textdomain,
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
