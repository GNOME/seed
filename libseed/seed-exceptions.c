/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

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
 * Copyright (C) Robert Carr 2009 <carrr@rpi.edu>
 */

#include "seed-private.h"
#include <stdarg.h>

/**
 * seed_make_exception:
 * @ctx: A #SeedContext.
 * @exception: A reference to a #SeedException in which to store the exception.
 * @name: The #gchar* representing the exception name.
 * @message: The #gchar*, as a printf format string, representing the
 *           details of the exception.
 * @VarArgs: A list of printf-style format arguments to substitute in @message.
 *
 * Creates a new JavaScript exception with the given attributes.
 *
 * The line number and file name of the exception created will be undefined.
 *
 */
void
seed_make_exception (JSContextRef ctx,
		     JSValueRef * exception,
		     const gchar * name, const gchar * message, ...)
{
  JSStringRef js_name = 0;
  JSStringRef js_message = 0;
  JSValueRef js_name_ref = 0, js_message_ref = 0;
  JSObjectRef exception_obj;
  gchar *mes;
  va_list args;

  if (!exception)
    return;

  va_start (args, message);

  if (name)
    {
      js_name = JSStringCreateWithUTF8CString (name);
      js_name_ref = JSValueMakeString (ctx, js_name);
    }
  if (message)
    {
      mes = g_strdup_vprintf (message, args);
      js_message = JSStringCreateWithUTF8CString (mes);
      js_message_ref = JSValueMakeString (ctx, js_message);
      g_free (mes);
    }

  // TODO: needs to create a global class named 'name', and this needs to
  // be an instance of it, for integration with normal JS!

  exception_obj = JSObjectMake (ctx, 0, NULL);
  seed_object_set_property (ctx, exception_obj, "message", js_message_ref);
  seed_object_set_property (ctx, exception_obj, "name", js_name_ref);

  *exception = exception_obj;

  JSStringRelease (js_name);
  JSStringRelease (js_message);

  va_end (args);
}

/**
 * seed_make_exception_from_gerror:
 * @ctx: A #SeedContext.
 * @exception: A reference to a #SeedException in which to store the exception.
 * @error: A #GError* from which to copy the properties of the exception.
 *
 * Generates @exception with the name and description of @error.
 *
 */
void
seed_make_exception_from_gerror (JSContextRef ctx,
				 JSValueRef * exception, GError * error)
{
  const gchar *domain = g_quark_to_string (error->domain);
  GString *string = g_string_new (domain);
  guint i;
  gsize len = string->len;

  *(string->str) = g_unichar_toupper (*(string->str));
  for (i = 0; i < len; i++)
    {
      if (*(string->str + i) == '-')
	{
	  *(string->str + i + 1) = g_unichar_toupper (*(string->str + i + 1));
	  g_string_erase (string, i, 1);
	}
      else if (!g_strcmp0 (string->str + i - 1, "Quark"))
	g_string_truncate (string, i - 1);

    }
  seed_make_exception (ctx, exception, string->str, error->message, NULL);

  g_string_free (string, TRUE);
}

/**
 * seed_exception_get_name:
 * @ctx: A #SeedContext.
 * @exception: A reference to a #SeedException.
 *
 * Retrieves the name of the given exception; this could be one of the
 * predefined exception names given above, or your own name, which should
 * be a single CamelCase word, preferably ending in something like "Error".
 *
 * Return value: A #gchar* representing the name of @exception.
 *
 */
gchar *
seed_exception_get_name (JSContextRef ctx, JSValueRef e)
{
  JSValueRef name;
  g_assert ((e));
  if (!JSValueIsObject (ctx, e))
    return NULL;

  name = seed_object_get_property (ctx, (JSObjectRef) e, "name");
  return seed_value_to_string (ctx, name, NULL);
}

/**
 * seed_exception_get_message:
 * @ctx: A #SeedContext.
 * @exception: A reference to a #SeedException.
 *
 * Retrieves the message of the given exception; this should be a
 * human-readable string describing the exception enough that a developer
 * could utilize the message in order to determine where to look to debug
 * the problem.
 *
 * Return value: A #gchar* representing the detailed message of @exception.
 *
 */
gchar *
seed_exception_get_message (JSContextRef ctx, JSValueRef e)
{
  JSValueRef name;
  g_assert ((e));
  if (!JSValueIsObject (ctx, e))
    return 0;

  name = seed_object_get_property (ctx, (JSObjectRef) e, "message");
  return seed_value_to_string (ctx, name, NULL);
}

/**
 * seed_exception_get_line:
 * @ctx: A #SeedContext.
 * @exception: A reference to a #SeedException.
 *
 * Retrieves the line number the given exception was thrown from; keep in mind
 * that exceptions created from C have an undefined line number.
 *
 * Return value: A #guint representing the line number from which @exception
 *               was thrown.
 *
 */
guint
seed_exception_get_line (JSContextRef ctx, JSValueRef e)
{
  JSValueRef line;
  g_assert ((e));
  if (!JSValueIsObject (ctx, e))
    return 0;
  line = seed_object_get_property (ctx, (JSObjectRef) e, "line");
  return seed_value_to_uint (ctx, line, NULL);
}

/**
 * seed_exception_get_file:
 * @ctx: A #SeedContext.
 * @exception: A reference to a #SeedException.
 *
 * Retrieves the file name the given exception was thrown from; keep in mind
 * that exceptions created from C have an undefined file name.
 *
 * Return value: A #gchar* representing the name of the file from which
 *               @exception was thrown.
 *
 */
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

/**
 * seed_exception_to_string:
 * @ctx: A #SeedContext.
 * @exception: A reference to a #SeedException.
 *
 * Properly formats the name, detailed message, line number, and file name of
 * the given extension. This provides a consistent format for printed
 * exceptions, to reduce confusion. Please use it if you're exposing exception
 * data to the outside world.
 *
 * Return value: A #gchar* representing the @exception.
 *
 */
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
