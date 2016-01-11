/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2 of
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
seed_value_protect (JSContextRef ctx, JSValueRef value)
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
seed_value_unprotect (JSContextRef ctx, JSValueRef value)
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
 * Create a new #SeedContext. By default, this creates a new context
 * which has no global objects; you can add the default set using
 * seed_prepare_global_context().
 *
 * Return value: A new #SeedContext.
 *
 */
JSGlobalContextRef
seed_context_create (JSContextGroupRef group, JSClassRef global_class)
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
 * seed_context_collect:
 * @ctx: A #SeedContext.
 *
 * Instructs JavaScriptCore to make a garbage collection pass.
 * The context parameter is currently unused, and a pass is made
 * through all contexts.
 *
 */
void
seed_context_collect (JSGlobalContextRef ctx)
{
  JSGarbageCollect(ctx);
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

JSValueRef
seed_object_get_property_at_index (JSContextRef ctx,
				   JSObjectRef object,
				   gint index, JSValueRef * exception)
{
  return JSObjectGetPropertyAtIndex (ctx, object, index, exception);
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
 *               in error messages, or 0.
 *
 * Creates a new #SeedScript instance with @js as the contents, then
 * checks for proper syntax.
 *
 * Note: seed_make_script() does not handle the shebang line, and will return a
 *       parse error if one is included in @js.
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
 * @exception: A #SeedException pointer to store an exception in.
 *
 * Evaluates a string of JavaScript in @ctx; if an exception
 * is raised in the context of the script, it will be placed in @exception.
 *
 * Return value: The #SeedValue returned by evaluating the script.
 *
 */
JSValueRef
seed_simple_evaluate (JSContextRef ctx,
		      const gchar * source, JSValueRef * exception)
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
 * Retrieves the exception (if any) raised during the evaluation of @s.
 *
 * Return value: A #SeedException representing the exception of @s.
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

void
seed_script_destroy (SeedScript * s)
{
  seed_string_unref (s->script);
  if (s->source_url)
    seed_string_unref (s->source_url);

  g_free (s);
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
 * Retrieves the private data of @object.
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
 * Determine whether or not @value represents the JavaScript null value
 *
 * Return value: A #gboolean (%true if @value is 'null')
 *
 */
gboolean
seed_value_is_null (JSContextRef ctx, JSValueRef value)
{
  return JSValueIsNull (ctx, value);
}

/**
 * seed_value_is_undefined:
 * @ctx: A #SeedContext.
 * @value: A #SeedValue.
 *
 * Determine whether or not @value represents the JavaScript undefined value
 *
 * Return value: A #gboolean (%true if @value is 'undefined')
 *
 */
gboolean
seed_value_is_undefined (JSContextRef ctx, JSValueRef value)
{
  return JSValueIsUndefined (ctx, value);
}

/**
 * seed_value_is_object:
 * @ctx: A #SeedContext.
 * @value: A #SeedValue.
 *
 * Determine whether or not @value is a JavaScript object
 *
 * Return value: A #gboolean (%true if @value is an object)
 *
 */
gboolean
seed_value_is_object (JSContextRef ctx, JSValueRef value)
{
  return !seed_value_is_null (ctx, value) && JSValueIsObject (ctx, value);
}

/**
 * seed_value_is_object_of_class:
 * @ctx: A #SeedContext.
 * @value: A #SeedValue.
 * @klass: A #SeedClass.
 *
 * Determine whether or not @value is an object of the specified class
 *
 * Return value: A #gboolean (%true if @value is of class @klass)
 *
 */
gboolean
seed_value_is_object_of_class (JSContextRef ctx, JSValueRef value,
			       JSClassRef klass)
{
  return !seed_value_is_null (ctx, value)
    && JSValueIsObjectOfClass (ctx, value, klass);
}

/**
 * seed_value_is_function:
 * @ctx: A #SeedContext.
 * @value: A #SeedValue.
 *
 * Determine whether or not @value is a JavaScript function (and, therefore,
 * an object)
 *
 * Return value: A #gboolean (%true if @value is a function)
 *
 */
gboolean
seed_value_is_function (JSContextRef ctx, JSObjectRef value)
{
  return seed_value_is_object (ctx, value) && JSObjectIsFunction (ctx, value);
}

/**
 * seed_value_is_string:
 * @ctx: A #SeedContext.
 * @value: A #SeedValue.
 *
 * Determine whether or not @value is a JavaScript string
 *
 * Return value: A #gboolean (%true if @value is a string)
 *
 */
gboolean
seed_value_is_string (JSContextRef ctx, JSValueRef value)
{
  return JSValueIsString (ctx, value);
}

/**
 * seed_value_is_number:
 * @ctx: A #SeedContext.
 * @value: A #SeedValue.
 *
 * Determine whether or not @value is a JavaScript number
 *
 * Return value: A #gboolean (%true if @value is a number)
 *
 */
gboolean
seed_value_is_number (JSContextRef ctx, JSValueRef value)
{
  return JSValueIsNumber (ctx, value);
}

/**
 * seed_engine_set_search_path:
 * @eng: A #SeedEngine, on which to set the path.
 * @path: A #const gchar*, a colon separated string containing the path to set
 *
 * Sets the search path for the imports system.
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
 * Retrieves the search path for the imports system. The returned value is
 * owned by the #SeedEngine, and shouldn't be freed by the application writer.
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
 *
 * Connects @function to the signal specified by @signal on @object. @user_data
 * is passed as the user_data argument to the callback function.
 *
 */
void
seed_signal_connect_full (JSContextRef ctx,
			  GObject * object,
			  const gchar * signal,
			  JSObjectRef function, JSObjectRef user_data)
{
  seed_gobject_signal_connect (ctx, signal, object, function,
			       NULL, user_data);
}

/**
 * seed_signal_connect:
 * @ctx: A valid #SeedContext
 * @object: A #GObject, to connect the signal on.
 * @signal: A signal specification.
 * @script: The script to connect to the signal. Should return a function.
 *
 * Evaluates @script, which should return a function, then connects the returned
 * function to the signal specified by @signal on @object. @user_data
 * is passed as the user_data argument to the callback function.
 *
 */
void
seed_signal_connect (JSContextRef ctx,
		     GObject * object,
		     const gchar * signal, const gchar * script)
{
  JSValueRef func;

  func = seed_simple_evaluate (ctx, script, NULL);
  seed_signal_connect_full (ctx, object, signal, (JSObjectRef) func, NULL);
}

/**
 * seed_context_get_global_object:
 * @ctx: A valid #SeedContext
 *
 * Return value: The global object for @ctx.
 */
JSObjectRef
seed_context_get_global_object (JSGlobalContextRef ctx)
{
  return JSContextGetGlobalObject (ctx);
}


/**
 * seed_make_array:
 * @ctx: A valid #SeedContext
 * @elements: An array of #SeedValue's with which to populate the array.
 * @num_elements: The number of values, in @elements
 * @exception: A #SeedException in which to store an exception.
 *             Pass %NULL to ignore exceptions.
 *
 * Creates a JavaScript Array object from @elements, a C-style array of
 * SeedValues.
 *
 * Return value: A new array object, populated with @elements.
 */

JSObjectRef
seed_make_array (JSContextRef ctx,
		 const JSValueRef elements[],
		 gsize num_elements, JSValueRef * exception)
{
  return JSObjectMakeArray (ctx, num_elements, elements, exception);
}

/**
 * seed_make_null:
 * @ctx: A valid #SeedContext
 *
 * Note that this function returns a valid SeedValue,
 * representing the null JavaScript value, and not a
 * null SeedValue.
 *
 * Return value: A #SeedValue of the 'null' type.
 *
 */
JSValueRef
seed_make_null (JSContextRef ctx)
{
  return JSValueMakeNull (ctx);
}

/**
 * seed_make_undefined:
 * @ctx: A valid #SeedContext
 *
 * Note that this function returns a valid SeedValue,
 * representing the undefined JavaScript value, and not an
 * undefined SeedValue.
 *
 * Return value: A #SeedValue of the 'undefined' type.
 */
JSValueRef
seed_make_undefined (JSContextRef ctx)
{
  return JSValueMakeUndefined (ctx);
}

/**
 * seed_value_get_type:
 * @ctx: A valid #SeedContext
 * @value: A #SeedValue
 *
 * Return value: The type of @value
 */
JSType
seed_value_get_type (JSContextRef ctx, JSValueRef value)
{
  return JSValueGetType (ctx, value);
}

/**
 * seed_object_copy_property_names:
 * @ctx: A valid #SeedContext
 * @object: An object from which to copy property names.
 *
 * Return value: A %NULL terminated array containing the property names of @object
 */
gchar **
seed_object_copy_property_names (JSContextRef ctx, JSObjectRef object)
{
  JSPropertyNameArrayRef names;
  JSStringRef name;
  guint i, length;
  gsize max_length;
  gchar *c_name;
  gchar **ret;

  names = JSObjectCopyPropertyNames (ctx, object);
  length = JSPropertyNameArrayGetCount (names);
  ret = (gchar **) g_malloc ((length + 1) * sizeof (gchar *));
  for (i = 0; i < length; i++)
    {
      name = JSPropertyNameArrayGetNameAtIndex (names, i);
      max_length = JSStringGetMaximumUTF8CStringSize (name);
      c_name = (gchar *) g_malloc (max_length * sizeof (gchar));
      JSStringGetUTF8CString (name, c_name, max_length);
      ret[i] = c_name;

    }
  ret[length] = NULL;
  JSPropertyNameArrayRelease (names);

  return ret;
}

/**
 * seed_object_get_prototype:
 * @ctx: A valid #SeedContext
 * @obj: A #SeedObject
 *
 * Return value: The prototype of @obj.
 */
JSObjectRef
seed_object_get_prototype (JSContextRef ctx, JSObjectRef obj)
{
  return (JSObjectRef) JSObjectGetPrototype (ctx, obj);
}

gboolean
seed_object_is_of_class (JSContextRef ctx, JSObjectRef obj, JSClassRef class)
{
  return JSValueIsObjectOfClass (ctx, obj, class);
}

/**
 * seed_make_function:
 * @ctx: A valid #SeedContext
 * @func: A #SeedFunctionCallback to implement the function.
 * @name: The name of the function (used in exceptions).
 *
 * Creates a JavaScript object representing a first-class function; when
 * the function is called from JavaScript, @func will be called.
 *
 * Return value: A #SeedObject representing the function
 */
JSObjectRef
seed_make_function (JSContextRef ctx, gpointer func, gchar * name)
{
  JSObjectRef oref;
  JSStringRef jsname = NULL;
  if (name)
    jsname = JSStringCreateWithUTF8CString (name);
  oref = JSObjectMakeFunctionWithCallback (ctx, NULL, func);

  if (jsname)
    JSStringRelease (jsname);

  return oref;
}

/**
 * seed_value_to_format:
 * @ctx: A valid #SeedContext
 * @format: Format string to use.
 * @exception: Location to store an exception.
 * @values: The values to convert.
 * @Varargs: A %NULL-terminated list of locations to store the results of conversion.
 *
 * A convenience API for converting multiple values at once, the format string
 * is composed of single characters specifying types, for example:
 * i: gint
 * u: guint
 * o: GObject *
 * s: gchar *
 * f: gdouble
 * c: gchar
 *
 * and a valid format string could be "iuo".
 *
 * This function may be in particular useful in converting arguments
 * in a #SeedFunctionCallback.
 * Return value: Whether conversion was successful.
 */
gboolean
seed_value_to_format (JSContextRef ctx,
		      const gchar * format,
		      JSValueRef * values, JSValueRef * exception, ...)
{
  va_list argp;
  const gchar *c;
  guint i = 0;

  va_start (argp, exception);

  for (c = format; *c; c++)
    {
      JSValueRef val = values[i];
      gpointer p = va_arg (argp, gpointer);

      if (!val || !p)
	{
	  va_end (argp);
	  return FALSE;
	}
      switch (*c)
	{
	case 'i':
	  {
	    *((gint *) p) = seed_value_to_int (ctx, val, exception);
	    break;
	  }
	case 'u':
	  {
	    *((guint *) p) = seed_value_to_uint (ctx, val, exception);
	    break;
	  }
	case 's':
	  {
	    *((gchar **) p) = seed_value_to_string (ctx, val, exception);
	    break;
	  }
	case 'f':
	  {
	    *((gdouble *) p) = seed_value_to_int (ctx, val, exception);
	    break;
	  }
	case 'o':
	  {
	    *((GObject **) p) = seed_value_to_object (ctx, val, exception);
	    break;
	  }
	case 'c':
	  {
	    *((gchar *) c) = seed_value_to_char (ctx, val, exception);
	    break;
	  }
	}
      i++;
    }

  va_end (argp);
  return TRUE;
}

/*************************** CALLBACK DOCUMENTATION **************************/

/**
 * SeedFunctionCallback:
 * @ctx: A #SeedContext
 * @function: The #SeedObject representing the function
 * @this_object: The #SeedObject representing the "this" object in the caller
 * @argument_count: The number of arguments passed into the callback
 * @arguments: An array of #SeedValues; the value of the arguments passed in
 * @exception: A reference to a #SeedException; use seed_make_exception() in order
 *             to throw a JavaScript exception from the callback.
 *
 * All native C function callbacks should use the prototype of
 * SeedFunctionCallback.
 *
 * Return value: The #SeedValue to return to the caller
 */
