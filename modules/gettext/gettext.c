#include <seed.h>
#include <glib/gi18n.h>

SeedObject namespace_ref;
SeedEngine * eng;

static SeedValue
seed_gettext_gettext (SeedContext ctx,
                      SeedObject function,
                      SeedObject this_object,
                      gsize argument_count,
                      const SeedValue arguments[],
                      SeedException * exception)
{
	gchar * msgid;
	SeedValue ret;

	if (argument_count != 1)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.gettext expected 1 argument, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	msgid = seed_value_to_string (ctx, arguments[0], exception);

	ret = seed_value_from_string (ctx, gettext(msgid), exception);
	g_free(ret);

	return ret;
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
	SeedValue ret;

	if (argument_count != 1)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.textdomain expected 1 argument, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	domain_name = seed_value_to_string (ctx, arguments[0], exception);

	ret = seed_value_from_string (ctx, textdomain(domain_name), exception);
	g_free(domain_name);

	return ret;
}

static SeedValue
seed_gettext_bindtextdomain (SeedContext ctx,
                             SeedObject function,
                             SeedObject this_object,
                             gsize argument_count,
                             const SeedValue args[],
                             SeedException * exception)
{
	gchar * domainname, * dirname;
	SeedValue ret;

	if (argument_count != 2)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.bindtextdomain expected 2 arguments, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	domainname = seed_value_to_string (ctx, args[0], exception);
	dirname = seed_value_to_string (ctx, args[1], exception);

	ret = seed_value_from_string (ctx,
	                              bindtextdomain(domainname, dirname),
	                              exception);
	g_free(domainname);
	g_free(dirname);

	return ret;
}

static SeedValue
seed_gettext_bind_textdomain_codeset (SeedContext ctx,
                                      SeedObject function,
                                      SeedObject this_object,
                                      gsize argument_count,
                                      const SeedValue args[],
                                      SeedException * exception)
{
	gchar * domainname, * codeset;
	SeedValue ret;

	if (argument_count != 2)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.bind_textdomain_codeset expected 2 arguments, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	domainname = seed_value_to_string (ctx, args[0], exception);
	codeset = seed_value_to_string (ctx, args[1], exception);

	ret = seed_value_from_string (ctx,
								  bind_textdomain_codeset(domainname, codeset),
								  exception);
	g_free(domainname);
	g_free(codeset);

	return ret;
}

static SeedValue
seed_gettext_dgettext (SeedContext ctx,
                       SeedObject function,
                       SeedObject this_object,
                       gsize argument_count,
                       const SeedValue args[],
                       SeedException * exception)
{
	gchar * domainname, * msgid;
	SeedValue ret;

	if (argument_count != 2)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.dgettext expected 2 arguments, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	domainname = seed_value_to_string (ctx, args[0], exception);
	msgid = seed_value_to_string (ctx, args[1], exception);

	ret = seed_value_from_string (ctx, dgettext(domainname, msgid), exception);
	g_free(domainname);
	g_free(msgid);

	return ret;
}

static SeedValue
seed_gettext_dcgettext (SeedContext ctx,
                        SeedObject function,
                        SeedObject this_object,
                        gsize argument_count,
                        const SeedValue args[],
                        SeedException * exception)
{
	gchar * domainname, * msgid;
	gint category;
	SeedValue ret;

	if (argument_count != 2)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.dcgettext expected 3 arguments, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	domainname = seed_value_to_string (ctx, args[0], exception);
	msgid = seed_value_to_string (ctx, args[1], exception);
	category = seed_value_to_int (ctx, args[2], exception);

	ret = seed_value_from_string (ctx,
	                              dcgettext(domainname, msgid, category),
	                              exception);
	g_free(domainname);
	g_free(msgid);
}

static SeedValue
seed_gettext_ngettext (SeedContext ctx,
                       SeedObject function,
                       SeedObject this_object,
                       gsize argument_count,
                       const SeedValue args[],
                       SeedException * exception)
{
	gchar * msgid, * msgid_plural;
	guint n;
	SeedValue ret;

	if (argument_count != 3)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.ngettext expected 3 arguments, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	msgid = seed_value_to_string (ctx, args[0], exception);
	msgid_plural = seed_value_to_string (ctx, args[1], exception);
	n = seed_value_to_uint (ctx, args[2], exception);

	ret = seed_value_from_string (ctx, ngettext(msgid, msgid_plural, n), exception);
	g_free(msgid);
	g_free(msgid_plural);

	return ret;
}

static SeedValue
seed_gettext_dngettext (SeedContext ctx,
                        SeedObject function,
                        SeedObject this_object,
                        gsize argument_count,
                        const SeedValue args[],
                        SeedException * exception)
{
	gchar * domainname, * msgid, * msgid_plural;
	guint n;
	SeedValue ret;

	if (argument_count != 4)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.dngettext expected 4 arguments, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	domainname = seed_value_to_string (ctx, args[0], exception);
	msgid = seed_value_to_string (ctx, args[1], exception);
	msgid_plural = seed_value_to_string (ctx, args[2], exception);
	n = seed_value_to_uint (ctx, args[3], exception);

	ret = seed_value_from_string (ctx, dngettext(domainname, msgid, msgid_plural, n), exception);
	g_free(domainname);
	g_free(msgid);
	g_free(msgid_plural);

	return ret;
}

static SeedValue
seed_gettext_dcngettext (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
	gchar * domainname, * msgid, * msgid_plural;
	guint n;
	gint category;
	SeedValue ret;

	if (argument_count != 5)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.dcngettext expected 5 arguments, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	domainname = seed_value_to_string (ctx, args[0], exception);
	msgid = seed_value_to_string (ctx, args[1], exception);
	msgid_plural = seed_value_to_string (ctx, args[2], exception);
	n = seed_value_to_uint (ctx, args[3], exception);
	category = seed_value_to_int (ctx, args[4], exception);

	ret = seed_value_from_string (ctx,
	                              dcngettext(domainname, msgid, msgid_plural, n, category),
	                              exception);
	g_free(domainname);
	g_free(msgid);
	g_free(msgid_plural);

	return ret;
}

static SeedValue
seed_gettext_setlocale (SeedContext ctx,
                        SeedObject function,
                        SeedObject this_object,
                        gsize argument_count,
                        const SeedValue args[],
                        SeedException * exception)
{
	gchar * locale;
	gint category;
	SeedValue ret;

	if (argument_count != 2)
	{
		seed_make_exception (ctx, exception, "ArgumentError",
		                     "gettext.setlocale expected 2 arguments, got %zd",
		                     argument_count);
		return seed_make_null (ctx);
	}

	category = seed_value_to_int (ctx, args[0], exception);
	locale = seed_value_to_string (ctx, args[1], exception);

	ret = seed_value_from_string (ctx,
	                              setlocale(category, locale),
	                              exception);
	g_free(locale);

	return ret;
}

SeedObject
seed_module_init(SeedEngine *local_eng)
{
	eng = local_eng;
	namespace_ref = seed_make_object (eng->context, NULL, NULL);
	seed_value_protect (eng->context, namespace_ref);

	seed_create_function(eng->context, "gettext",
	                     (SeedFunctionCallback) seed_gettext_gettext,
	                     namespace_ref);

	seed_create_function(eng->context, "textdomain",
	                     (SeedFunctionCallback) seed_gettext_textdomain,
	                     namespace_ref);

	seed_create_function(eng->context, "bindtextdomain",
	                     (SeedFunctionCallback) seed_gettext_bindtextdomain,
	                     namespace_ref);

	seed_create_function(eng->context, "bind_textdomain_codeset",
	                     (SeedFunctionCallback) seed_gettext_bind_textdomain_codeset,
	                     namespace_ref);

	seed_create_function(eng->context, "dgettext",
	                     (SeedFunctionCallback) seed_gettext_dgettext,
	                     namespace_ref);

	seed_create_function(eng->context, "dcgettext",
	                     (SeedFunctionCallback) seed_gettext_dcgettext,
	                     namespace_ref);

	seed_create_function(eng->context, "ngettext",
	                     (SeedFunctionCallback) seed_gettext_ngettext,
	                     namespace_ref);

	seed_create_function(eng->context, "dngettext",
	                     (SeedFunctionCallback) seed_gettext_dngettext,
	                     namespace_ref);

	seed_create_function(eng->context, "dcngettext",
	                     (SeedFunctionCallback) seed_gettext_dcngettext,
	                     namespace_ref);

	seed_create_function(eng->context, "setlocale",
	                     (SeedFunctionCallback) seed_gettext_setlocale,
	                     namespace_ref);

    /* define enums for setlocale. Where to put them?  */
    seed_object_set_property(eng->context, namespace_ref, "LC_CTYPE",
                             seed_value_from_long(eng->context, LC_CTYPE, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_NUMERIC",
                             seed_value_from_long(eng->context, LC_NUMERIC, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_TIME",
                             seed_value_from_long(eng->context, LC_TIME, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_COLLATE",
                             seed_value_from_long(eng->context, LC_COLLATE, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_MONETARY",
                             seed_value_from_long(eng->context, LC_MONETARY, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_MESSAGES",
                             seed_value_from_long(eng->context, LC_MESSAGES, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_ALL",
                             seed_value_from_long(eng->context, LC_ALL, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_PAPER",
                             seed_value_from_long(eng->context, LC_PAPER, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_NAME",
                             seed_value_from_long(eng->context, LC_NAME, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_ADDRESS",
                             seed_value_from_long(eng->context, LC_ADDRESS, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_TELEPHONE",
                             seed_value_from_long(eng->context, LC_TELEPHONE, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_MEASUREMENT",
                             seed_value_from_long(eng->context, LC_MEASUREMENT, NULL));
    seed_object_set_property(eng->context, namespace_ref, "LC_IDENTIFICATION",
                             seed_value_from_long(eng->context, LC_IDENTIFICATION, NULL));

	return namespace_ref;
}

