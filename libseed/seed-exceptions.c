/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * seed-exceptions.c
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 *
 * libseed is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libseed is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "seed-private.h"

void
seed_make_exception(JSValueRef * exception,
					const gchar * name, const gchar * message)
{
	JSStringRef js_name = 0;
	JSStringRef js_message = 0;
	JSValueRef js_name_ref = 0, js_message_ref = 0;
	JSObjectRef exception_obj;

	if (!exception)
		return;

	if (name)
	{
		js_name = JSStringCreateWithUTF8CString(name);
		js_name_ref = JSValueMakeString(eng->context, js_name);
	}
	if (message)
	{
		js_message = JSStringCreateWithUTF8CString(message);
		js_message_ref = JSValueMakeString(eng->context, js_message);
	}

	exception_obj = JSObjectMake(eng->context, 0, NULL);
	seed_object_set_property(exception_obj, "message", js_message_ref);
	seed_object_set_property(exception_obj, "name", js_name_ref);

	*exception = exception_obj;

	JSStringRelease(js_name);
	JSStringRelease(js_message);
}

void seed_make_exception_from_gerror(JSValueRef * exception, GError * error)
{
	const gchar *domain = g_quark_to_string(error->domain);
	GString *string = g_string_new(domain);
	int i;

	*(string->str) = g_unichar_toupper(*(string->str));
	for (i = 0; i < string->len; i++)
	{
		if (*(string->str + i) == '-')
		{
			*(string->str + i + 1) = g_unichar_toupper(*(string->str + i + 1));
			g_string_erase(string, i, 1);
		}
		else if (!strcmp(string->str + i - 1, "Quark"))
			g_string_truncate(string, i - 1);

	}
	seed_make_exception(exception, string->str, error->message);

	g_string_free(string, TRUE);
}

gchar *seed_exception_get_name(JSValueRef e)
{
	JSValueRef name;
	g_assert((e));
	if (!JSValueIsObject(eng->context, e))
		return 0;

	name = seed_object_get_property((JSObjectRef) e, "name");
	return seed_value_to_string(name, 0);
}

gchar *seed_exception_get_message(JSValueRef e)
{
	JSValueRef name;
	g_assert((e));
	if (!JSValueIsObject(eng->context, e))
		return 0;

	name = seed_object_get_property((JSObjectRef) e, "message");
	return seed_value_to_string(name, 0);
}

guint seed_exception_get_line(JSValueRef e)
{
	JSValueRef line;
	g_assert((e));
	if (!JSValueIsObject(eng->context, e))
		return 0;
	line = seed_object_get_property((JSObjectRef) e, "line");
	return seed_value_to_uint(line, 0);
}

gchar *seed_exception_get_file(JSValueRef e)
{
	JSValueRef line;
	g_assert((e));
	if (!JSValueIsObject(eng->context, e))
		return 0;
	line = seed_object_get_property((JSObjectRef) e, "sourceURL");
	return seed_value_to_string(line, 0);
}

gchar *seed_exception_to_string(JSValueRef e)
{
	guint line;
	gchar *mes, *name, *file, *ret;

	line = seed_exception_get_line(e);
	mes = seed_exception_get_message(e);
	file = seed_exception_get_file(e);
	name = seed_exception_get_name(e);

	ret = g_strdup_printf("Line %d in %s: %s %s", line, file, name, mes);

	g_free(mes);
	g_free(file);
	g_free(name);

	return ret;
}
