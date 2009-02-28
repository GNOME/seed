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

void
seed_value_protect (JSContextRef ctx, JSValueRef value)
{
  JSValueProtect (ctx, value);
}

void
seed_value_unprotect (JSContextRef ctx, JSValueRef value)
{
  JSValueUnprotect (ctx, value);
}

/**
 * seed_context_create:
 * @group: A #SeedContextGroup in which to create the new context, or %NULL to create it in the default context group.
 * @global_class: The #SeedClass to use to create the global object, or %NULL to create it with the default class.
 *
 * Return value: A new #SeedContext.
 *
 */
JSGlobalContextRef
seed_context_create (JSContextGroupRef group, JSClassRef global_class)
{
  return JSGlobalContextCreateInGroup (group, global_class);
}

JSGlobalContextRef
seed_context_ref (JSGlobalContextRef ctx)
{
  return JSGlobalContextRetain (ctx);
}

void
seed_context_unref (JSGlobalContextRef ctx)
{
  JSGlobalContextRelease (ctx);
}

/**
 * seed_make_null:
 * @ctx: A #SeedContext.
 *
 * Return value: A #SeedValue representing %NULL.
 *
 */
JSValueRef
seed_make_null (JSContextRef ctx)
{
  return JSValueMakeNull (ctx);
}

/**
 * seed_make_object:
 * @ctx: The #SeedContext in which to create the new object.
 * @class: The #SeedClass to use to create the new object, or %NULL to use the default object class.
 * @private: The initial private data of the new object.
 *
 * Return value: A new #SeedObject.
 *
 */
JSObjectRef
seed_make_object (JSContextRef ctx, JSClassRef class, gpointer private)
{
  return JSObjectMake (ctx, class, private);
}

/**
 * seed_object_set_property_at_index:
 * @ctx: A #SeedContext.
 * @object: A #SeedObject on which to set the property.
 * @index: The index of the property to set.
 * @value: The #SeedValue to use as the property's value.
 * @exception: A reference to a #SeedValue in which to store any exceptions. Pass %NULL to ignore exceptions.
 *
 * Sets the property @index on @object to @value.
 *
 */
void
seed_object_set_property_at_index (JSContextRef ctx,
				   JSObjectRef object,
				   gint index,
				   JSValueRef value, JSValueRef * exception)
{
  JSObjectSetPropertyAtIndex (ctx, object, index, value, exception);
}

JSValueRef
seed_object_call (JSContextRef ctx,
		  JSObjectRef object,
		  JSObjectRef this,
		  size_t argument_count,
		  const JSValueRef arguments[], JSValueRef * exception)
{
  return JSObjectCallAsFunction (ctx, object, this,
				 argument_count, arguments, exception);
}

/**
 * seed_make_script:
 * @ctx: A #SeedContext.
 * @js: A string representing the contents of the script.
 * @source_url: The filename of the script, for reference in error messages, or %NULL.
 * @line_number: The line number of the beginning of the script, for reference in error messages, or %NULL.
 *
 * Creates a new #SeedScript instance with @js as the contents, then checks for proper syntax.
 *
 * Return value: The newly created #SeedScript.
 *
 */
SeedScript *
seed_make_script (JSContextRef ctx,
		  const gchar * js,
		  const gchar * source_url, gint line_number)
{
  SeedScript *ret = g_new0 (SeedScript, 1);

  if (js)
    ret->script = JSStringCreateWithUTF8CString (js);
  else
    ret->script = JSStringCreateWithUTF8CString ("");

  if (source_url)
    {
      ret->source_url = JSStringCreateWithUTF8CString (source_url);
    }
  ret->line_number = line_number;

  JSCheckScriptSyntax (ctx, ret->script,
		       ret->source_url, ret->line_number, &ret->exception);

  return ret;
}

/**
 * seed_script_new_from_file:
 * @ctx: A #SeedContext.
 * @file: The filename of the script to load.
 *
 * Uses seed_make_script() to create a #SeedScript from the contents of @file.
 *
 * Return value: The newly created #SeedScript.
 *
 */
SeedScript *
seed_script_new_from_file (JSContextRef ctx, gchar * file)
{
  SeedScript *script;
  GError *e = NULL;
  gchar *contents = NULL;

  g_file_get_contents (file, &contents, NULL, &e);
  script = seed_make_script (ctx, contents, file, 0);
  if (e)
    {
      seed_make_exception_from_gerror (ctx, &script->exception, e);
      g_error_free (e);
    }

  return script;
}

/**
 * seed_evaluate:
 * @ctx: A #SeedContext.
 * @s: A #SeedScript to evaluate.
 * @this: The object which should be assigned to the "this" global.
 *
 * Evaluates a #SeedScript with @this as the global "this" object.
 *
 * Return value: The #SeedValue returned by evaluating the script.
 *
 */
JSValueRef
seed_evaluate (JSContextRef ctx, SeedScript * s, JSObjectRef this)
{
  JSValueRef ret;

  s->exception = 0;
  ret = JSEvaluateScript (ctx,
			  s->script, this, s->source_url,
			  s->line_number, &s->exception);

  return ret;
}

/**
 * seed_simple_evaluate:
 * @ctx: A #SeedContext.
 * @source: A string representing the JavaScript to evaluate.
 *
 * Evaluates a string of JavaScript.
 *
 * Return value: The #SeedValue returned by evaluating the script.
 *
 */
JSValueRef
seed_simple_evaluate (JSContextRef ctx, gchar * source)
{
  JSValueRef ret;
  JSStringRef script = JSStringCreateWithUTF8CString (source);

  ret = JSEvaluateScript (ctx, script, NULL, NULL, 0, NULL);

  JSStringRelease (script);
  return ret;
}

/**
 * seed_script_exception:
 * @s: A #SeedScript.
 *
 * Return value: A #JSValueRef representing the exception of @s.
 *
 */
JSValueRef
seed_script_exception (SeedScript * s)
{
  return s->exception;
}

/**
 * seed_string_get_maximum_size:
 * @string: A #SeedString.
 *
 * Return value: The maximum number of bytes @string will take up if converted to a null-terminated UTF8 string.
 *
 */
gsize
seed_string_get_maximum_size (JSStringRef string)
{
  return JSStringGetMaximumUTF8CStringSize (string);
}

/**
 * seed_string_to_utf8_buffer:
 * @string: A #SeedString.
 * @buffer: An allocated string.
 * @buffer_size: The length of @buffer, in bytes.
 *
 * Return value: A the number of bytes copied into @buffer.
 *
 */
gsize
seed_string_to_utf8_buffer (JSStringRef string, gchar * buffer,
			    size_t buffer_size)
{
  return JSStringGetUTF8CString (string, buffer, buffer_size);
}

gboolean
seed_string_is_equal (JSStringRef a, JSStringRef b)
{
  return JSStringIsEqual (a, b);
}

gboolean
seed_string_is_equal_utf8 (JSStringRef a, const gchar * b)
{
  return JSStringIsEqualToUTF8CString (a, b);
}

JSStringRef
seed_string_ref (JSStringRef string)
{
  return JSStringRetain (string);
}

void
seed_string_unref (JSStringRef string)
{
  JSStringRelease (string);
}

JSClassRef
seed_create_class (JSClassDefinition * def)
{
  return JSClassCreate (def);
}

JSObjectRef
seed_make_constructor (JSContextRef ctx,
		       JSClassRef class,
		       JSObjectCallAsConstructorCallback constructor)
{
  return JSObjectMakeConstructor (ctx, class, constructor);
}

gpointer
seed_object_get_private (JSObjectRef object)
{
  return (gpointer) JSObjectGetPrivate (object);
}

void
seed_object_set_private (JSObjectRef object, gpointer value)
{
  JSObjectSetPrivate (object, value);
}

gboolean
seed_value_is_null (JSContextRef ctx, JSValueRef value)
{
  return JSValueIsNull (ctx, value);
}

gboolean
seed_value_is_object (JSContextRef ctx, JSValueRef value)
{
  return !seed_value_is_null (ctx, value) && JSValueIsObject (ctx, value);
}

gboolean
seed_value_is_function (JSContextRef ctx, JSObjectRef value)
{
  return seed_value_is_object (ctx, value) && JSObjectIsFunction (ctx, value);
}

/**
 * seed_engine_set_search_path:
 * @eng: A #SeedEngine, on which to set the path.
 * @path: A #gchar**, a null-terminated array of strings containing path to set.
 *
 */
void
seed_engine_set_search_path (SeedEngine * eng, gchar ** path)
{
  /* this should be null from seed_init unless there's already a path set. */
  g_free (eng->search_path);
  eng->search_path = g_strdupv (path);
}

/**
 * seed_engine_get_search_path:
 * @eng: A #SeedEngine, to get the currently set search path.
 *
 * Return value: A null-terminated array of strings containing the paths.
 *
 */
gchar **
seed_engine_get_search_path (SeedEngine * eng)
{
  return eng->search_path;
}
