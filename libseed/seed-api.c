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

/**
 * seed_value_protect:
 * @ctx: A #SeedContext.
 * @value: The #SeedValue to protect.
 *
 * Increments the "protection count" of @value, in case you want to store
 * a reference somewhere where the garbage collector won't be able to find it,
 * and don't want it to be collected!
 *
 * In order for @value to be collected afterwards, it will need to be
 * unprotected the same number of times with seed_value_unprotect().
 *
 */
void
seed_value_protect (JSContextRef ctx,
		    JSValueRef value)
{
  JSValueProtect (ctx, value);
}

/**
 * seed_value_unprotect:
 * @ctx: A #SeedContext.
 * @value: The #SeedValue to unprotect.
 *
 * Decrements the "protection count" of @value, as explained in 
 * seed_value_protect().
 *
 */
void
seed_value_unprotect (JSContextRef ctx, 
		      JSValueRef value)
{
  JSValueUnprotect (ctx, value);
}

/**
 * seed_context_create:
 * @group: A #SeedContextGroup in which to create the new context, or %NULL to
 *         create it in the default context group.
 * @global_class: The #SeedClass to use to create the global object, or %NULL to
 *                create it with the default class.
 *
 * Return value: A new #SeedContext.
 *
 */
JSGlobalContextRef
seed_context_create (JSContextGroupRef group, 
		     JSClassRef global_class)
{
  return JSGlobalContextCreateInGroup (group, global_class);
}

/**
 * seed_context_ref:
 * @ctx: A #SeedContext.
 *
 * Increments the reference count of @ctx.
 *
 * Return value: @ctx
 *
 */
JSGlobalContextRef
seed_context_ref (JSGlobalContextRef ctx)
{
  return JSGlobalContextRetain (ctx);
}

/**
 * seed_context_unref:
 * @ctx: A #SeedContext.
 *
 * Decrements the reference count of @ctx.
 *
 */
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
 * @class: The #SeedClass to use to create the new object, or %NULL to use the
 *         default object class.
 * @private: The initial private data of the new object.
 *
 * Return value: A new #SeedObject.
 *
 */
JSObjectRef
seed_make_object (JSContextRef ctx,
		  JSClassRef class, 
		  gpointer private)
{
  return JSObjectMake (ctx, class, private);
}

/**
 * seed_object_set_property_at_index:
 * @ctx: A #SeedContext.
 * @object: A #SeedObject on which to set the property.
 * @index: The index of the property to set.
 * @value: The #SeedValue to use as the property's value.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
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

/**
 * seed_object_call
 * @ctx: A #SeedContext.
 * @object: A #SeedObject to call.
 * @this: The #SeedObject to use as the 'this' object inside the called function.
 * @argument_count: The number of arguments in the @arguments array.
 * @arguments: An array (@argument_count long) of #SeedValues to pass in as the
 *             function's arguments. 
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Calls @object as a function.
 *
 * Return value: The @SeedValue returned by the called function, or %NULL if an
 *               exception occurs or the object is not a function.
 *
 */
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
 * @source_url: The filename of the script, for reference in errors, or %NULL.
 * @line_number: The line number of the beginning of the script, for reference
 *               in error messages, or %NULL.
 *
 * Creates a new #SeedScript instance with @js as the contents, then
 * checks for proper syntax.
 *
 * Return value: The newly created #SeedScript.
 *
 */
SeedScript *
seed_make_script (JSContextRef ctx,
		  const gchar * js,
		  const gchar * source_url,
		  gint line_number)
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
seed_script_new_from_file (JSContextRef ctx, 
			   gchar * file)
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
seed_evaluate (JSContextRef ctx,
	       SeedScript * s,
	       JSObjectRef this)
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
 * @exception: A #SeedException pointer to store an exception in.
 *
 * Evaluates a string of JavaScript.
 *
 * Return value: The #SeedValue returned by evaluating the script.
 *
 */
JSValueRef
seed_simple_evaluate (JSContextRef ctx, 
		      const gchar * source,
		      JSValueRef *exception)
{
  JSValueRef ret;
  JSStringRef script = JSStringCreateWithUTF8CString (source);

  ret = JSEvaluateScript (ctx, script, NULL, NULL, 0, exception);

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
 * Return value: The maximum number of bytes @string will take up if converted
 *               to a null-terminated UTF8 string.
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

/**
 * seed_string_is_equal:
 * @a: The first #SeedString to compare.
 * @b: The second #SeedString to compare.
 *
 * Return value: #true, if a and b are equal, #false otherwise.
 *
 */
gboolean
seed_string_is_equal (JSStringRef a, JSStringRef b)
{
  return JSStringIsEqual (a, b);
}

/**
 * seed_string_is_equal_utf8:
 * @a: The #SeedString to compare.
 * @b: A #gchar* to compare to.
 *
 * Tests the equality of a SeedString and a UTF-8 C-style string.
 *
 * Return value: #true, if a and b are equal, #false otherwise.
 *
 */
gboolean
seed_string_is_equal_utf8 (JSStringRef a, const gchar * b)
{
  return JSStringIsEqualToUTF8CString (a, b);
}

/**
 * seed_string_ref:
 * @string: A #SeedString.
 *
 * Increments the reference count of @string.
 *
 * Return value: @string
 *
 */
JSStringRef
seed_string_ref (JSStringRef string)
{
  return JSStringRetain (string);
}

/**
 * seed_string_unref:
 * @string: A #SeedString.
 *
 * Decrements the reference count of @string.
 *
 */
void
seed_string_unref (JSStringRef string)
{
  JSStringRelease (string);
}

// TODO:FIXME: Do we have an external typedef or anything for JSClassDefinition?

/**
 * seed_create_class:
 * @def: A #JSClassDefinition.
 *
 * Return value: A #SeedClass, described by @def. 
 *
 */
JSClassRef
seed_create_class (JSClassDefinition * def)
{
  return JSClassCreate (def);
}

/* TODO:FIXME: GtkDoc is choking on JSObjectCallAsConstructorCallback, and 
               merging it into the parameter name...??! */

/**
 * seed_make_constructor:
 * @ctx: A #SeedContext.
 * @class: A #SeedClass to use as the default for constructed objects.
 * @constructor: The #JSObjectCallAsConstructorCallback function to call when
 *               the constructor is invoked with 'new'.
 *
 * Return value: A #SeedObject, which is a constructor function.
 *
 */
JSObjectRef
seed_make_constructor (JSContextRef ctx,
		       JSClassRef class,
		       JSObjectCallAsConstructorCallback constructor)
{
  return JSObjectMakeConstructor (ctx, class, constructor);
}

/**
 * seed_object_get_private:
 * @object: A #SeedObject.
 *
 * Return value: A pointer to the private data of @object.
 *
 */
gpointer
seed_object_get_private (JSObjectRef object)
{
  return (gpointer) JSObjectGetPrivate (object);
}

/**
 * seed_object_set_private:
 * @object: A #SeedObject.
 * @value: A #gpointer to set the private data of @object to.
 *
 * Sets the private data of @object to @value.
 *
 */
void
seed_object_set_private (JSObjectRef object, gpointer value)
{
  JSObjectSetPrivate (object, value);
}

/**
 * seed_value_is_null:
 * @ctx: A #SeedContext.
 * @value: A #SeedValue.
 *
 * Return value: #true if @value represents %NULL, #false otherwise.
 *
 */
gboolean
seed_value_is_null (JSContextRef ctx, JSValueRef value)
{
  return JSValueIsNull (ctx, value);
}

/**
 * seed_value_is_object:
 * @ctx: A #SeedContext.
 * @value: A #SeedValue.
 *
 * Return value: #true if @value is an object, #false otherwise.
 *
 */
gboolean
seed_value_is_object (JSContextRef ctx, JSValueRef value)
{
  return !seed_value_is_null (ctx, value) && JSValueIsObject (ctx, value);
}

/**
 * seed_value_is_function:
 * @ctx: A #SeedContext.
 * @value: A #SeedObject.
 *
 * Return value: #true if @value is a function (and therefore, an object),
 *               #false otherwise.
 *
 */
gboolean
seed_value_is_function (JSContextRef ctx, JSObjectRef value)
{
  return seed_value_is_object (ctx, value) && JSObjectIsFunction (ctx, value);
}

/**
 * seed_engine_set_search_path:
 * @eng: A #SeedEngine, on which to set the path.
 * @path: A #const gchar*, a colon separated string containing the path to set
 *
 * Sets the default search path for Seed.include.
 *
 */
void
seed_engine_set_search_path (SeedEngine * eng, const gchar * path)
{
  /* this should be null from seed_init unless there's already a path set. */

  g_strfreev (eng->search_path);
  eng->search_path = g_strsplit (path, ":", -1);
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

/**
 * seed_signal_connect_full:
 * @ctx: A valid #SeedContext
 * @object: A #GObject, to connect the signal on.
 * @signal: A signal specification.
 * @function: The JavaScript function to connect to the signal.
 * @user_data: An additional parameter to pass to the function.
 */
void
seed_signal_connect_full (JSContextRef ctx,
			   GObject *object,
			   const gchar *signal,
			   JSObjectRef function,
			   JSObjectRef user_data)			   
{
  seed_gobject_signal_connect(ctx, signal, object, function,
			       NULL, user_data);
}

/**
 * seed_signal_connect:
 * @ctx: A valid #SeedContext
 * @object: A #GObject, to connect the signal on.
 * @signal: A signal specification.
 * @script: The script to connect to the signal. Should be a function.
 */
void
seed_signal_connect (JSContextRef ctx,
		     GObject *object,
		     const gchar *signal,
		     const gchar *script)
{
  JSValueRef func;
  
  func = seed_simple_evaluate(ctx, script, NULL);
  seed_signal_connect_full(ctx, object, signal, (JSObjectRef)func,
			   NULL);
}

JSObjectRef
seed_context_get_global_object (JSGlobalContextRef ctx)
{
  return JSContextGetGlobalObject(ctx);
}


JSObjectRef
seed_make_array (JSContextRef ctx, 
		 const JSValueRef elements[],
		 gsize num_elements, 
		 JSValueRef *exception)
{
  return JSObjectMakeArray (ctx, num_elements, elements, exception);
}

JSValueRef
seed_make_undefined (JSContextRef ctx)
{
  return JSValueMakeUndefined (ctx);
}

JSType
seed_value_get_type (JSContextRef ctx,
		     JSValueRef value)
{
  return JSValueGetType (ctx, value);
}

gchar **
seed_object_copy_property_names(JSContextRef ctx,
				JSObjectRef object)
{
  JSPropertyNameArrayRef names;
  guint i, length;
  gchar **ret;
  
  names = JSObjectCopyPropertyNames (ctx, object);
  length = JSPropertyNameArrayGetCount (names);
  ret = g_malloc((length+1)*sizeof(gchar *));
  for (i = 0; i < length; i++)
    {
      JSStringRef name = JSPropertyNameArrayGetNameAtIndex (names, i);
      guint max_length;
      gchar *c_name;
      
      max_length = JSStringGetMaximumUTF8CStringSize (name);
      c_name = g_malloc (max_length * sizeof (gchar));
      JSStringGetUTF8CString (name, c_name, length);
      ret[i] = c_name;
      
    }
  ret[length] = NULL;
  JSPropertyNameArrayRelease (names);
  
  return ret;  
}
