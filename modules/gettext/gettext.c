#include <seed-module.h>

#include <glib/gi18n.h>
#include <locale.h>

SeedObject ns_ref;
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

	CHECK_ARG_COUNT("gettext.gettext", 1);

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

	CHECK_ARG_COUNT("gettext.textdomain", 1);

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

	CHECK_ARG_COUNT("gettext.bindtextdomain", 2);

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

	CHECK_ARG_COUNT("gettext.bind_textdomain_codeset", 2);

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

	CHECK_ARG_COUNT("gettext.dgettext", 2);

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

	CHECK_ARG_COUNT("gettext.dcgettext", 3);

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

	CHECK_ARG_COUNT("gettext.ngettext", 3);

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

	CHECK_ARG_COUNT("gettext.dngettext", 4);

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

	CHECK_ARG_COUNT("gettext.dcngettext", 5);

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

	CHECK_ARG_COUNT("gettext.setlocale", 2);

	category = seed_value_to_int (ctx, args[0], exception);
	locale = seed_value_to_string (ctx, args[1], exception);

	ret = seed_value_from_string (ctx,
	                              setlocale(category, locale),
	                              exception);
	g_free(locale);

	return ret;
}

seed_static_function gettext_funcs[] = {
	{"gettext", seed_gettext_gettext, 0},
	{"textdomain", seed_gettext_textdomain, 0},
	{"bindtextdomain", seed_gettext_bindtextdomain, 0},
	{"bind_textdomain_codeset", seed_gettext_bind_textdomain_codeset, 0},
	{"dgettext", seed_gettext_dgettext, 0},
	{"dcgettext", seed_gettext_dcgettext, 0},
	{"ngettext", seed_gettext_ngettext, 0},
	{"dngettext", seed_gettext_dngettext, 0},
	{"dcngettext", seed_gettext_dcngettext, 0},
	{"setlocale", seed_gettext_setlocale, 0},
	{"gettext", seed_gettext_gettext, 0}
};

SeedObject
seed_module_init(SeedEngine *local_eng)
{
	SeedGlobalContext ctx = local_eng->context;


	seed_class_definition gettext_ns_class_def = seed_empty_class;
	SeedClass gettext_ns_class = seed_create_class(&gettext_ns_class_def);

	ns_ref = seed_make_object (ctx, gettext_ns_class, NULL);
	seed_value_protect (ctx, ns_ref);

	/* define enums for setlocale. Where to put them?  */
	
	DEFINE_ENUM_MEMBER(ns_ref, LC_CTYPE);
	DEFINE_ENUM_MEMBER(ns_ref, LC_NUMERIC);
	DEFINE_ENUM_MEMBER(ns_ref, LC_TIME);
	DEFINE_ENUM_MEMBER(ns_ref, LC_COLLATE);
	DEFINE_ENUM_MEMBER(ns_ref, LC_MONETARY);
	DEFINE_ENUM_MEMBER(ns_ref, LC_MESSAGES);
	DEFINE_ENUM_MEMBER(ns_ref, LC_ALL);
	DEFINE_ENUM_MEMBER(ns_ref, LC_PAPER);
	DEFINE_ENUM_MEMBER(ns_ref, LC_NAME);
	DEFINE_ENUM_MEMBER(ns_ref, LC_ADDRESS);
	DEFINE_ENUM_MEMBER(ns_ref, LC_TELEPHONE);
	DEFINE_ENUM_MEMBER(ns_ref, LC_MEASUREMENT);
	DEFINE_ENUM_MEMBER(ns_ref, LC_IDENTIFICATION);
	DEFINE_ENUM_MEMBER(ns_ref, LC_CTYPE);

	return ns_ref;
}

