/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version. 
 * Seed is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Lesser General Public License for more details. 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with Seed.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 */

#include "seed-private.h"
#include <string.h>

void
seed_make_exception (JSContextRef ctx,
		     JSValueRef * exception,
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
      js_name = JSStringCreateWithUTF8CString (name);
      js_name_ref = JSValueMakeString (ctx, js_name);
    }
  if (message)
    {
      js_message = JSStringCreateWithUTF8CString (message);
      js_message_ref = JSValueMakeString (ctx, js_message);
    }

  exception_obj = JSObjectMake (ctx, 0, NULL);
  seed_object_set_property (ctx, exception_obj, "message", js_message_ref);
  seed_object_set_property (ctx, exception_obj, "name", js_name_ref);

  *exception = exception_obj;

  JSStringRelease (js_name);
  JSStringRelease (js_message);
}

void
seed_make_exception_from_gerror (JSContextRef ctx,
				 JSValueRef * exception, GError * error)
{
  const gchar *domain = g_quark_to_string (error->domain);
  GString *string = g_string_new (domain);
  int i;

  *(string->str) = g_unichar_toupper (*(string->str));
  for (i = 0; i < string->len; i++)
    {
      if (*(string->str + i) == '-')
	{
	  *(string->str + i + 1) = g_unichar_toupper (*(string->str + i + 1));
	  g_string_erase (string, i, 1);
	}
      else if (!strcmp (string->str + i - 1, "Quark"))
	g_string_truncate (string, i - 1);

    }
  seed_make_exception (ctx, exception, string->str, error->message);

  g_string_free (string, TRUE);
}

gchar *
seed_exception_get_name (JSContextRef ctx, JSValueRef e)
{
  JSValueRef name;
  g_assert ((e));
  if (!JSValueIsObject (ctx, e))
    return 0;

  name = seed_object_get_property (ctx, (JSObjectRef) e, "name");
  return seed_value_to_string (ctx, name, 0);
}

gchar *
seed_exception_get_message (JSContextRef ctx, JSValueRef e)
{
  JSValueRef name;
  g_assert ((e));
  if (!JSValueIsObject (ctx, e))
    return 0;

  name = seed_object_get_property (ctx, (JSObjectRef) e, "message");
  return seed_value_to_string (ctx, name, 0);
}

guint
seed_exception_get_line (JSContextRef ctx, JSValueRef e)
{
  JSValueRef line;
  g_assert ((e));
  if (!JSValueIsObject (ctx, e))
    return 0;
  line = seed_object_get_property (ctx, (JSObjectRef) e, "line");
  return seed_value_to_uint (ctx, line, 0);
}

gchar *
seed_exception_get_file (JSContextRef ctx, JSValueRef e)
{
  JSValueRef line;
  g_assert ((e));
  if (!JSValueIsObject (ctx, e))
    return 0;
  line = seed_object_get_property (ctx, (JSObjectRef) e, "sourceURL");
  return seed_value_to_string (ctx, line, 0);
}

gchar *
seed_exception_to_string (JSContextRef ctx, JSValueRef e)
{
  guint line;
  gchar *mes, *name, *file, *ret;

  line = seed_exception_get_line (ctx, e);
  mes = seed_exception_get_message (ctx, e);
  file = seed_exception_get_file (ctx, e);
  name = seed_exception_get_name (ctx, e);

  ret = g_strdup_printf ("Line %d in %s: %s %s", line, file, name, mes);

  g_free (mes);
  g_free (file);
  g_free (name);

  return ret;
}
