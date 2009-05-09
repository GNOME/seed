/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/* Copyright 2008 litl, LLC. All Rights Reserved. */

#include "dbus-values.h"

#include <seed.h>

//#include <util/dbus.h>
//#include <util/log.h>

#include <string.h>

gboolean
seed_js_one_value_from_dbus (SeedContext ctx,
			     DBusMessageIter * iter, 
                             SeedValue *value_p,
                             SeedException *exception)
{
  int arg_type;

  *value_p = seed_make_unefined (ctx);

  arg_type = dbus_message_iter_get_arg_type (iter);

  //  big_debug_marshal(BIG_DEBUG_JS_DBUS,
  //                "Converting dbus type '%c' to SeedValue",
  //                arg_type != DBUS_TYPE_INVALID ? arg_type : '0');

  switch (arg_type)
    {
    case DBUS_TYPE_STRUCT:
      {
	SeedObject obj;
	DBusMessageIter struct_iter;
	int index;

	obj = seed_make_object (ctx, NULL, NULL);

	dbus_message_iter_recurse (iter, &struct_iter);
	index = 0;
	while (dbus_message_iter_get_arg_type (&struct_iter) !=
	       DBUS_TYPE_INVALID)
	  {
	    SeedValue prop_value;

	    prop_value = seed_make_undefined (ctx);

	    if (!seed_js_one_value_from_dbus
		(context, &struct_iter, &prop_value, exception))
	      {
                return FALSE;
	      }

              seed_object_set_property_at_index (ctx, obj, index,
                                                 prop_value, exception);
              dbus_message_iter_next (&struct_iter);
              index++;
	  }
	*value_p = (SeedValue) obj;
      }
      break;
    case DBUS_TYPE_ARRAY:
      {
	int elem_type = dbus_message_iter_get_element_type (iter);

	if (elem_type == DBUS_TYPE_DICT_ENTRY)
	  {
	    /* Create a dictionary object */
            SeedObject obj;
	    DBusMessageIter array_iter;

	    obj = seed_make_object (ctx, NULL, NULL);

	    dbus_message_iter_recurse (iter, &array_iter);
	    while (dbus_message_iter_get_arg_type (&array_iter) !=
		   DBUS_TYPE_INVALID)
	      {
		DBusMessageIter entry_iter;
		const char *key;
		SeedValue entry_value;

		dbus_message_iter_recurse (&array_iter, &entry_iter);

		if (dbus_message_iter_get_arg_type (&entry_iter) !=
		    DBUS_TYPE_STRING)
		  {
		    seed_make_exception (ctx,
                                         "Dictionary keys are not strings,"
                                         "can't convert to JavaScript",
                                         exception);
                    return FALSE;
		  }

		dbus_message_iter_get_basic (&entry_iter, &key);

		dbus_message_iter_next (&entry_iter);

                //		big_debug_marshal (BIG_DEBUG_JS_DBUS,
		//		   "Defining dict entry %s in jsval dict",
		//		   key);

		entry_value = seed_make_undefined (ctx);

		if (!seed_js_one_value_from_dbus
		    (ctx, &entry_iter, &entry_value, exception))
		  {
		    return FALSE;
		  }
                
                seed_object_set_property (ctx, obj,
                                          key, entry_value);

		dbus_message_iter_next (&array_iter);
	      }

	    *value_p = (SeedValue) obj;
	  }
	else if (elem_type == DBUS_TYPE_BYTE)
	  {
	    /* byte arrays go to a string */
	    const char *v_BYTES;
	    int n_bytes;
	    DBusMessageIter array_iter;

	    dbus_message_iter_recurse (iter, &array_iter);
	    dbus_message_iter_get_fixed_array (&array_iter,
					       &v_BYTES, &n_bytes);
            // This is where I am.
            *value_p = seed_value_from_binary_string (ctx, v_BYTES, n_bytes, exception);
	  }
	else
	  {
            SeedObject obj;
	    DBusMessageIter array_iter;
	    int index;

            obj = seed_make_object (ctx, NULL, NULL);

	    dbus_message_iter_recurse (iter, &array_iter);
	    index = 0;
	    while (dbus_message_iter_get_arg_type (&array_iter) !=
		   DBUS_TYPE_INVALID)
	      {
		SeedValue prop_value;

		prop_value = seed_make_undefined (ctx);

		if (!seed_js_one_value_from_dbus
		    (context, &array_iter, &prop_value, exception))
		  {
                    return FALSE;
		  }
                
                seed_object_set_property_at_index (ctx, obj, index, prop_value, exception);

		JS_RemoveRoot (context, &prop_value);
		dbus_message_iter_next (&array_iter);
		index++;
	      }
	    *value_p = (SeedValue) obj;
	  }
      }
      break;
    case DBUS_TYPE_BOOLEAN:
      {
	dbus_bool_t v_BOOLEAN;
	dbus_message_iter_get_basic (iter, &v_BOOLEAN);
	*value_p = seed_value_from_boolean (ctx, v_BOOLEAN, exception);
      }
      break;
    case DBUS_TYPE_BYTE:
      {
	unsigned char v_BYTE;
	dbus_message_iter_get_basic (iter, &v_BYTE);
        *value_p = seed_value_from_int (ctx, v_BYTE, exception);
      }
      break;
    case DBUS_TYPE_INT32:
      {
	dbus_int32_t v_INT32;
	dbus_message_iter_get_basic (iter, &v_INT32);
        *value_p = seed_value_from_int (ctx, v_INT32, exception);
      }
      break;
    case DBUS_TYPE_UINT32:
      {
	dbus_uint32_t v_UINT32;
	dbus_message_iter_get_basic (iter, &v_UINT32);
        *value_p = seed_value_from_uint (ctx, v_UINT32, exception);
      }
      break;
    case DBUS_TYPE_INT64:
      {
	dbus_int64_t v_INT64;
	dbus_message_iter_get_basic (iter, &v_INT64);
        *value_p = seed_value_from_int64 (ctx, v_INT64, exception);
      }
      break;
    case DBUS_TYPE_UINT64:
      {
	dbus_uint64_t v_UINT64;
	dbus_message_iter_get_basic (iter, &v_UINT64);
        *value_p = seed_value_from_uint64 (ctx, v_UINT64, exception);
      }
      break;
    case DBUS_TYPE_DOUBLE:
      {
	double v_DOUBLE;
	dbus_message_iter_get_basic (iter, &v_DOUBLE);
        *value_p = seed_value_from_double (ctx, v_DOUBLE, exception);
      }
      break;
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_STRING:
      {
	const char *v_STRING;

	dbus_message_iter_get_basic (iter, &v_STRING);

        *value_p = seed_value_from_string (ctx, v_STRING, exception);
      }
      break;

    case DBUS_TYPE_VARIANT:
      {
	DBusMessageIter variant_iter;

	dbus_message_iter_recurse (iter, &variant_iter);

	return seed_js_one_value_from_dbus (context, &variant_iter, value_p, exception);
      }
      break;

    case DBUS_TYPE_INVALID:
      *value_p = seed_make_undefined (ctx);
      break;

    default:/*
      big_debug (BIG_DEBUG_JS_DBUS,
		 "Don't know how to convert dbus type %c to JavaScript",
		 arg_type);
      gjs_throw (context,
		 "Don't know how to convert dbus type %c to JavaScript",
		 arg_type);*/
      return FALSE;
    }

  return TRUE;
}

gboolean
seed_js_values_from_dbus (SeedContext ctx,
			  DBusMessageIter * iter, 
                          GArray **array_p,
                          SeedException *exception)
{
  GArray *array;
  SeedValue value;
  
  value = seed_make_undefined (ctx);
  
  array = g_array_new (FALSE, FALSE, sizeof(SeedValue));

  /* TODO */
  // THIS IS WHERE IA M
  // Wheee.
  do
    {
      if (!seed_js_one_value_from_dbus (context, iter, &value))
	{
          g_array_free (array, TRUE);
          return FALSE;
	}
      
      g_array_append_val (array, value);
    }
  while (dbus_message_iter_next (iter));
                       
  *array_p = array;
  return TRUE;
}

static void
append_basic_maybe_in_variant (DBusMessageIter * iter,
			       int dbus_type,
			       void *value, gboolean wrap_in_variant)
{
  if (wrap_in_variant)
    {
      char buf[2];
      DBusMessageIter variant_iter;

      buf[0] = dbus_type;
      buf[1] = '\0';

      dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, buf,
					&variant_iter);
      dbus_message_iter_append_basic (&variant_iter, dbus_type, value);
      dbus_message_iter_close_container (iter, &variant_iter);
    }
  else
    {
      dbus_message_iter_append_basic (iter, dbus_type, value);
    }
}

static void
append_byte_array_maybe_in_variant (DBusMessageIter * iter,
				    const char *data,
				    gsize len, 
                                    gboolean wrap_in_variant)
{
  DBusMessageIter array_iter;
  DBusMessageIter variant_iter;

  if (wrap_in_variant)
    {
      dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "ay",
					&variant_iter);
    }

  dbus_message_iter_open_container (wrap_in_variant ? &variant_iter : iter,
				    DBUS_TYPE_ARRAY, "y", &array_iter);

  dbus_message_iter_append_fixed_array (&array_iter, DBUS_TYPE_BYTE,
					&data, len);

  dbus_message_iter_close_container (wrap_in_variant ? &variant_iter : iter,
				     &array_iter);

  if (wrap_in_variant)
    {
      dbus_message_iter_close_container (iter, &variant_iter);
    }
}

static gboolean
append_string (SeedContext ctx,
	       DBusMessageIter * iter,
	       const char *forced_signature, 
               const char *s, 
               gsize len,
               SeedException *exception)
{
  int forced_type;

  if (forced_signature == NULL || *forced_signature == DBUS_TYPE_INVALID)
    forced_type = DBUS_TYPE_STRING;
  else
    forced_type = *forced_signature;

  switch (forced_type)
    {
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_SIGNATURE:
      append_basic_maybe_in_variant (iter, forced_type, &s, FALSE);
      break;
    case DBUS_TYPE_VARIANT:
      append_basic_maybe_in_variant (iter, DBUS_TYPE_STRING, &s, TRUE);
      break;
    case DBUS_TYPE_ARRAY:
      g_assert (forced_signature != NULL);
      g_assert (forced_signature[0] == DBUS_TYPE_ARRAY);
      if (forced_signature[1] == DBUS_TYPE_BYTE)
	{
	  append_byte_array_maybe_in_variant (iter, s, len, FALSE);
	}
      else
	{
	  seed_make_exception (ctx, exception,
		     "JavaScript string can't be converted to dbus array with elements of type '%c'",
		     forced_signature[1]);
	  return FALSE;
	}
      break;
    default:
      seed_make_exception (ctx, exception,
		 "JavaScript string can't be converted to dbus type '%c'",
		 forced_type);
      return FALSE;
    }

  return TRUE;
}

static gboolean
append_int32 (SeedContext ctx,
	      DBusMessageIter * iter, 
              int forced_type, 
              dbus_int32_t v_INT32,
              SeedException *exception)
{
  if (forced_type == DBUS_TYPE_INVALID)
    forced_type = DBUS_TYPE_INT32;

  switch (forced_type)
    {
    case DBUS_TYPE_INT32:
      append_basic_maybe_in_variant (iter, forced_type, &v_INT32, FALSE);
      break;
    case DBUS_TYPE_VARIANT:
      append_basic_maybe_in_variant (iter, DBUS_TYPE_INT32, &v_INT32, TRUE);
      break;
    case DBUS_TYPE_UINT32:
      {
	dbus_uint32_t v_UINT32 = v_INT32;
	append_basic_maybe_in_variant (iter, forced_type, &v_UINT32, FALSE);
      }
      break;
    case DBUS_TYPE_DOUBLE:
      {
	double v_DOUBLE = v_INT32;
	append_basic_maybe_in_variant (iter, forced_type, &v_DOUBLE, FALSE);
      }
      break;
    case DBUS_TYPE_BYTE:
      {
	unsigned char v_BYTE = v_INT32;
	append_basic_maybe_in_variant (iter, forced_type, &v_BYTE, FALSE);
      }
      break;
    default:
      seed_make_exception_exception (ctx, exception,
		 "JavaScript Integer can't be converted to dbus type '%c'",
		 forced_type);
      return FALSE;
    }

  return TRUE;
}

static gboolean
append_double (SeedContext ctx,
	       DBusMessageIter * iter, int forced_type, double v_DOUBLE,
               SeedException *exception)
{
  if (forced_type == DBUS_TYPE_INVALID)
    forced_type = DBUS_TYPE_DOUBLE;

  switch (forced_type)
    {
    case DBUS_TYPE_DOUBLE:
      append_basic_maybe_in_variant (iter, forced_type, &v_DOUBLE, FALSE);
      break;
    case DBUS_TYPE_VARIANT:
      append_basic_maybe_in_variant (iter, DBUS_TYPE_DOUBLE, &v_DOUBLE, TRUE);
      break;
    case DBUS_TYPE_INT32:
      {
	dbus_int32_t v_INT32 = v_DOUBLE;
	append_basic_maybe_in_variant (iter, forced_type, &v_INT32, FALSE);
      }
      break;
    case DBUS_TYPE_UINT32:
      {
	dbus_uint32_t v_UINT32 = v_DOUBLE;
	append_basic_maybe_in_variant (iter, forced_type, &v_UINT32, FALSE);
      }
      break;
    default:
      seed_make_exception (ctx, exception,
                           "JavaScript Number can't be converted to dbus type '%c'",
                           forced_type);
      return FALSE;
    }

  return TRUE;
}

static gboolean
append_boolean (SeedContext ctx,
		DBusMessageIter * iter,
		int forced_type, dbus_bool_t v_BOOLEAN,
                SeedException *exception)
{
  if (forced_type == DBUS_TYPE_INVALID)
    forced_type = DBUS_TYPE_BOOLEAN;

  switch (forced_type)
    {
    case DBUS_TYPE_BOOLEAN:
      append_basic_maybe_in_variant (iter, forced_type, &v_BOOLEAN, FALSE);
      break;
    case DBUS_TYPE_VARIANT:
      append_basic_maybe_in_variant (iter, DBUS_TYPE_BOOLEAN, &v_BOOLEAN,
				     TRUE);
      break;
    default:
      seed_make_exception (ctx, exception,
                           "JavaScript Boolean can't be converted to dbus type '%c'",
                           forced_type);
      return FALSE;
    }

  return TRUE;
}

static gboolean
append_array (SeedContext ctx,
	      DBusMessageIter * iter,
	      DBusSignatureIter * sig_iter, SeedObject array, 
              int length,
              SeedException *exception)
{
  DBusSignatureIter element_sig_iter;
  int forced_type;
  SeedValue element;
  DBusMessageIter array_iter;
  DBusMessageIter variant_iter;
  int i;
  char *sig;

  forced_type = dbus_signature_iter_get_current_type (sig_iter);

  if (forced_type == DBUS_TYPE_VARIANT)
    {
      DBusSignatureIter variant_sig_iter;

      dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT,
					"av", &variant_iter);
      dbus_signature_iter_init (&variant_sig_iter, "av");
      if (!append_array (context, &variant_iter,
			 &variant_sig_iter, array, length))
	return FALSE;
      dbus_message_iter_close_container (iter, &variant_iter);

      return TRUE;
    }
  else if (forced_type != DBUS_TYPE_ARRAY)
    {
      seed_make_exception (ctx, exception,
		 "JavaScript Array can't be converted to dbus type %c",
		 forced_type);
      return FALSE;
    }

  g_assert (dbus_signature_iter_get_current_type (sig_iter) ==
	    DBUS_TYPE_ARRAY);
  dbus_signature_iter_recurse (sig_iter, &element_sig_iter);

  sig = dbus_signature_iter_get_signature (&element_sig_iter);
  dbus_message_iter_open_container (iter, DBUS_TYPE_ARRAY, sig, &array_iter);
  dbus_free (sig);

  for (i = 0; i < length; i++)
    {
      element = seed_object_get_property_at_index (ctx, array, i, exception);

      //      big_debug_marshal (BIG_DEBUG_JS_DBUS, " Adding array element %u", i);

      if (!seed_js_one_value_to_dbus (context, element, &array_iter,
				      &element_sig_iter))
	return FALSE;
    }

  dbus_message_iter_close_container (iter, &array_iter);

  return TRUE;
}
// TODO: THIS IS WHERE I AM
static gboolean
append_dict (SeedContext ctx,
	     DBusMessageIter * iter,
	     DBusSignatureIter * sig_iter, 
             SeedObject props,
             SeedException *exception)
{
  DBusSignatureIter element_sig_iter;
  int forced_type;
  DBusMessageIter variant_iter;
  SeedObject props_iter;
  DBusMessageIter dict_iter;
  DBusSignatureIter dict_value_sig_iter;
  char *sig;
  SeedValue prop_signatures;

  forced_type = dbus_signature_iter_get_current_type (sig_iter);

  if (forced_type == DBUS_TYPE_VARIANT)
    {
      DBusSignatureIter variant_sig_iter;

      dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT,
					"a{sv}", &variant_iter);
      dbus_signature_iter_init (&variant_sig_iter, "a{sv}");
      if (!append_dict (context, &variant_iter, &variant_sig_iter, props))
	return FALSE;
      dbus_message_iter_close_container (iter, &variant_iter);

      return TRUE;
    }
  else if (forced_type != DBUS_TYPE_ARRAY)
    {
      seed_make_exception (ctx, exception,
                           "JavaScript Object can't be converted to dbus type %c",
                           forced_type);
      return FALSE;
    }

  g_assert (dbus_signature_iter_get_current_type (sig_iter) ==
	    DBUS_TYPE_ARRAY);
  dbus_signature_iter_recurse (sig_iter, &element_sig_iter);

  if (dbus_signature_iter_get_current_type (&element_sig_iter) !=
      DBUS_TYPE_DICT_ENTRY)
    {
      seed_make_exception (ctx, exception,
                           "Objects must be marshaled as array of dict entry not of %c",
                           dbus_signature_iter_get_current_type (&element_sig_iter));
      return FALSE;
    }

  /* dbus itself enforces that dict keys are strings */

  g_assert (dbus_signature_iter_get_current_type (&element_sig_iter) ==
	    DBUS_TYPE_DICT_ENTRY);
  dbus_signature_iter_recurse (&element_sig_iter, &dict_value_sig_iter);
  /* check it points to key type first */
  g_assert (dbus_signature_iter_get_current_type (&dict_value_sig_iter) ==
	    DBUS_TYPE_STRING);
  /* move to value type */
  dbus_signature_iter_next (&dict_value_sig_iter);

  sig = dbus_signature_iter_get_signature (&element_sig_iter);
  dbus_message_iter_open_container (iter, DBUS_TYPE_ARRAY, sig, &dict_iter);
  dbus_free (sig);

  /* If a dictionary contains another dictionary at key
   * _dbus_signatures, the sub-dictionary can provide the signature
   * of each value in the outer dictionary. This allows forcing
   * integers to unsigned or whatever.
   *
   * _dbus_signatures has a weird name to avoid conflicting with
   * real properties. Matches _dbus_sender which is used elsewhere.
   *
   * We don't bother rooting the signature object or the stuff in it
   * because we assume the outer dictionary is rooted so the stuff
   * in it is also.
   */
  prop_signatures = seed_object_get_property (ctx, props, "_dbus_signatures");

  if (!seed_value_is_undefined (ctx, prop_signatures) && !seed_value_is_object (ctx, prop_signatures))
    {
      seed_make_exception (ctx, exception, "_dbus_signatures prop must be an object");
      return FALSE;
    }

  if (!seed_value_is_undefined (ctx, prop_signatures) &&
      dbus_signature_iter_get_current_type (&dict_value_sig_iter) !=
      DBUS_TYPE_VARIANT)
    {
      seed_make_exception (ctx, exception,
                           "Specifying _dbus_signatures for a dictionary with non-variant values is useless");
      return FALSE;
    }

  props_iter = JS_NewPropertyIterator (context, props);
  if (props_iter == NULL)
    {
      gjs_throw (context,
		 "Failed to create property iterator for object props");
      return FALSE;
    }

  prop_id = JSVAL_VOID;
  if (!JS_NextProperty (context, props_iter, &prop_id))
    return FALSE;

  while (prop_id != JSVAL_VOID)
    {
      SeedValue nameval;
      char *name;
      SeedValue propval;
      DBusMessageIter entry_iter;
      const char *value_signature;

      if (!JS_IdToValue (context, prop_id, &nameval))
	return FALSE;

      if (!gjs_string_to_utf8 (context, nameval, &name))
	return FALSE;

      if (strcmp (name, "_dbus_signatures") == 0)
	{
	  /* skip the magic "_dbus_signatures" field */
	  goto next;
	}

      /* see if this prop has a forced signature */
      value_signature = NULL;
      if (prop_signatures != JSVAL_VOID)
	{
	  SeedValue signature_value;
	  signature_value = JSVAL_VOID;
	  gjs_object_get_property (context,
				   JSVAL_TO_OBJECT (prop_signatures),
				   name, &signature_value);
	  if (signature_value != JSVAL_VOID)
	    {
	      value_signature = gjs_string_get_ascii_checked (context,
							      signature_value);
	      if (value_signature == NULL)
		{
		  return FALSE;
		}
	    }
	}

      if (!gjs_object_require_property
	  (context, props, "DBus append_dict", name, &propval))
	return FALSE;

      big_debug_marshal (BIG_DEBUG_JS_DBUS, " Adding property %s", name);

      /* seed_js_one_value_to_dbus() would check this also, but would not
       * print the property name, which is often useful
       */
      if (JSVAL_IS_NULL (propval))
	{
	  gjs_throw (context,
		     "Property '%s' has a null value, can't send over dbus",
		     name);
	  return FALSE;
	}

      dbus_message_iter_open_container (&dict_iter, DBUS_TYPE_DICT_ENTRY,
					NULL, &entry_iter);

      dbus_message_iter_append_basic (&entry_iter, DBUS_TYPE_STRING, &name);
      g_free (name);

      if (value_signature != NULL)
	{
	  DBusSignatureIter forced_signature_iter;
	  DBusMessageIter variant_iter;

	  g_assert (dbus_signature_iter_get_current_type
		    (&dict_value_sig_iter) == DBUS_TYPE_VARIANT);

	  dbus_message_iter_open_container (&entry_iter,
					    DBUS_TYPE_VARIANT,
					    value_signature, &variant_iter);

	  dbus_signature_iter_init (&forced_signature_iter, value_signature);

	  if (!seed_js_one_value_to_dbus (context, propval, &variant_iter,
					  &forced_signature_iter))
	    return FALSE;

	  dbus_message_iter_close_container (&entry_iter, &variant_iter);
	}
      else
	{
	  if (!seed_js_one_value_to_dbus (context, propval, &entry_iter,
					  &dict_value_sig_iter))
	    return FALSE;
	}

      dbus_message_iter_close_container (&dict_iter, &entry_iter);

    next:
      prop_id = JSVAL_VOID;
      if (!JS_NextProperty (context, props_iter, &prop_id))
	return FALSE;
    }

  dbus_message_iter_close_container (iter, &dict_iter);

  return TRUE;
}

gboolean
seed_js_one_value_to_dbus (SeedContext ctx,
			   SeedValue value,
			   DBusMessageIter * iter,
			   DBusSignatureIter * sig_iter)
{
  int forced_type;

  forced_type = dbus_signature_iter_get_current_type (sig_iter);

  big_debug_marshal (BIG_DEBUG_JS_DBUS,
		     "Converting dbus type '%c' from SeedValue",
		     forced_type != DBUS_TYPE_INVALID ? forced_type : '0');

  /* Don't write anything on the bus if the signature is empty */
  if (forced_type == DBUS_TYPE_INVALID)
    return TRUE;

  if (SEEDVALUE_IS_NULL (value))
    {
      big_debug (BIG_DEBUG_JS_DBUS, "Can't send null values over dbus");
      gjs_throw (context, "Can't send null values over dbus");
      return FALSE;
    }
  else if (SEEDVALUE_IS_STRING (value))
    {
      char *data;
      gsize len;
      char buf[3] = { '\0', '\0', '\0' };
      if (forced_type == DBUS_TYPE_ARRAY)
	{
	  buf[0] = DBUS_TYPE_ARRAY;
	  buf[1] = dbus_signature_iter_get_element_type (sig_iter);
	}
      else
	{
	  buf[0] = forced_type;
	}

      data = NULL;
      len = 0;
      if (buf[1] == DBUS_TYPE_BYTE)
	{
	  if (!gjs_string_get_binary_data (context, value, &data, &len))
	    return FALSE;
	}
      else
	{
	  if (!gjs_string_to_utf8 (context, value, &data))
	    return FALSE;
	  len = strlen (data);
	}

      if (!append_string (context, iter, buf, data, len))
	{
	  g_free (data);
	  return FALSE;
	}

      g_free (data);
    }
  else if (SEEDVALUE_IS_INT (value))
    {
      dbus_int32_t v_INT32;
      if (!JS_ValueToInt32 (context, value, &v_INT32))
	return FALSE;

      if (!append_int32 (context, iter, forced_type, v_INT32))
	return FALSE;
    }
  else if (SEEDVALUE_IS_DOUBLE (value))
    {
      double v_DOUBLE;
      if (!JS_ValueToNumber (context, value, &v_DOUBLE))
	return FALSE;

      if (!append_double (context, iter, forced_type, v_DOUBLE))
	return FALSE;
    }
  else if (SEEDVALUE_IS_BOOLEAN (value))
    {
      gboolean v_JS_BOOLEAN;
      dbus_bool_t v_BOOLEAN;
      if (!JS_ValueToBoolean (context, value, &v_JS_BOOLEAN))
	return FALSE;
      v_BOOLEAN = v_JS_BOOLEAN != FALSE;

      if (!append_boolean (context, iter, forced_type, v_BOOLEAN))
	return FALSE;
    }
  else if (SEEDVALUE_IS_OBJECT (value))
    {
      JSObject *obj;
      SeedValue lengthval;

      obj = SEEDVALUE_TO_OBJECT (value);

      /* see if there's a length property */
      gjs_object_get_property (context, obj, "length", &lengthval);

      if (SEEDVALUE_IS_INT (lengthval))
	{
	  guint length;

	  length = SEEDVALUE_TO_INT (lengthval);

	  big_debug_marshal (BIG_DEBUG_JS_DBUS,
			     "Looks like an array length %u", length);
	  if (!append_array (context, iter, sig_iter, obj, length))
	    return FALSE;
	}
      else
	{
	  big_debug_marshal (BIG_DEBUG_JS_DBUS, "Looks like a dictionary");
	  if (!append_dict (context, iter, sig_iter, obj))
	    return FALSE;
	}
    }
  else if (value == SEEDVALUE_VOID)
    {
      big_debug (BIG_DEBUG_JS_DBUS,
		 "Can't send void (undefined) values over dbus");
      gjs_throw (context, "Can't send void (undefined) values over dbus");
      return FALSE;
    }
  else
    {
      big_debug (BIG_DEBUG_JS_DBUS,
		 "Don't know how to convert this jsval to dbus type");
      gjs_throw (context,
		 "Don't know how to convert this jsval to dbus type");
      return FALSE;
    }

  return TRUE;
}

gboolean
seed_js_values_to_dbus (SeedContext ctx,
			int index,
			jsval values,
			DBusMessageIter * iter, DBusSignatureIter * sig_iter)
{
  jsval value;
  jsuint length;

  if (!JS_GetArrayLength (context, JSVAL_TO_OBJECT (values), &length))
    {
      gjs_throw (context, "Error retrieving length property of args array");
      return FALSE;
    }

  if (index > (int) length)
    {
      gjs_throw (context, "Index %d is bigger than array length %d", index,
		 length);
      return FALSE;
    }

  if (index == (int) length)
    return TRUE;

  if (!JS_GetElement (context, JSVAL_TO_OBJECT (values), index, &value))
    {
      gjs_throw (context, "Error accessing element %d of args array", index);
      return FALSE;
    }

  if (!seed_js_one_value_to_dbus (context, value, iter, sig_iter))
    {
      gjs_throw (context, "Error marshalling js value to dbus");
      return FALSE;
    }

  if (dbus_signature_iter_next (sig_iter))
    {
      return seed_js_values_to_dbus (context, index + 1, values, iter,
				     sig_iter);
    }

  return TRUE;
}

/* If jsval is an object, add properties from the DBusMessage such as the
 * sender. If jsval is not an object, do nothing.
 */
gboolean
seed_js_add_dbus_props (SeedContext ctx, DBusMessage * message, jsval value)
{
  const char *sender;

  if (!JSVAL_IS_OBJECT (value))
    return TRUE;

  sender = dbus_message_get_sender (message);

  if (!JS_DefineProperty (context, JSVAL_TO_OBJECT (value),
			  "_dbus_sender",
			  STRING_TO_JSVAL (JS_NewStringCopyZ
					   (context, sender)), NULL, NULL,
			  JSPROP_ENUMERATE))
    return FALSE;

  return TRUE;
}
