#include "dbus-exports.h"
#include "dbus-values.h"

#include <seed.h>
#include <string.h>

#include <util/dbus.h>

typedef struct _Exports {
  // Why does GJS have this?
  SeedObject object;
  
  DBusBusType which_bus;
  DBusConnection *connection_weak_ref;
  gboolean filter_was_registered;
} Exports;

SeedClass seed_js_exports_class;

static void              on_bus_opened        (DBusConnection *connection,
                                               void           *data);
static void              on_bus_closed        (DBusConnection *connection,
                                               void           *data);
static DBusHandlerResult on_message           (DBusConnection *connection,
                                               DBusMessage    *message,
                                               void           *user_data);

static const BigDBusConnectFuncs system_connect_funcs = {
    DBUS_BUS_SYSTEM,
    on_bus_opened,
    on_bus_closed
};

static const BigDBusConnectFuncs session_connect_funcs = {
    DBUS_BUS_SESSION,
    on_bus_opened,
    on_bus_closed
};

static void
on_bus_opened(DBusConnection *connection,
              void           *data)
{
    Exports *priv = data;

    g_assert(priv->connection_weak_ref == NULL);

    priv->connection_weak_ref = connection;

    if (priv->filter_was_registered)
        return;

    if (!dbus_connection_add_filter(connection,
                                    on_message, priv,
                                    NULL)) 
      {
	g_warning("DBus: Failed to add message filter");
	return;
      }
    
    priv->filter_was_registered = TRUE;
}

static void
on_bus_closed(DBusConnection *connection,
              void           *data)
{
    Exports *priv = data;

    g_assert(priv->connection_weak_ref != NULL);

    priv->connection_weak_ref = NULL;

    if (priv->filter_was_registered) 
      {
        dbus_connection_remove_filter(connection,
                                      on_message, priv);
        priv->filter_was_registered = FALSE;
      }
}

#define dbus_reply_from_exception(ctx, message, reply_p, exception)	\
        (dbus_reply_from_exception_and_sender((ctx), \
                                              dbus_message_get_sender(message), \
                                              dbus_message_get_serial(message), \
                                              (reply_p), exception))

static gboolean
dbus_reply_from_exception_and_sender(SeedContext  ctx,
                                     const gchar  *sender,
                                     dbus_uint32_t serial,
                                     DBusMessage **reply_p,
				     SeedException *exception)
{
  SeedValue name_val;
  gchar *s;
  const gchar *name = NULL;

  
  *reply_p = NULL;

  if (seed_value_is_undefined (ctx, *exception) || 
      seed_value_is_null (ctx, *exception) ||
      !seed_value_is_object (ctx, *exception))
    return FALSE;

  name_val = seed_object_get_property(ctx, *exception, "dbusErrorName");
  name = seed_value_to_string (ctx, name_val, NULL);

  s = seed_exception_to_string (ctx, *exception);
  g_warning("JS exception we will send as dbus reply to %s: %s",
	    sender,
	    s);

    *reply_p = dbus_message_new(DBUS_MESSAGE_TYPE_ERROR);
    dbus_message_set_destination(*reply_p, sender);
    dbus_message_set_reply_serial(*reply_p, serial);
    dbus_message_set_no_reply(*reply_p, TRUE);
    dbus_message_set_error_name(*reply_p, name ? name : DBUS_ERROR_FAILED);
    if (s != NULL) {
         DBusMessageIter iter;

        dbus_message_iter_init_append(*reply_p, &iter);

        if (!dbus_message_iter_append_basic(&iter,
                                            DBUS_TYPE_STRING,
                                            &s)) {
            dbus_message_unref(*reply_p);
            g_free(s);
            return FALSE;
        }
        g_free(s);
    }

    return TRUE;
}

// Is this going to leak later?
static gboolean
signature_from_method(SeedContext ctx,
		      SeedObject method_obj,
                      const char **signature,
		      SeedException *exception)
{
  SeedValue signature_value;
  
  if ((signature_value = seed_object_get_property(ctx,
						 method_obj, "outSignature")))
    {
      *signature = seed_value_to_string (ctx, signature_value, exception);
      if (*signature == NULL) 
	{
	  return FALSE;
        }
    } 
  else 
    {
      /* We default to a{sv} */
      *signature = "a{sv}";
    }
  
  return TRUE;
}

static gboolean
signature_has_one_element(const char *signature)
{
    DBusSignatureIter iter;

    if (!signature)
        return FALSE;

    dbus_signature_iter_init(&iter, signature);

    return !dbus_signature_iter_next(&iter);
}

static DBusMessage *
build_reply_from_jsval(SeedContext ctx,
                       const char    *signature,
                       const char    *sender,
                       dbus_uint32_t  serial,
		       SeedValue rval,
		       SeedException *exception)
{
    DBusMessage *reply;
    DBusMessageIter arg_iter;
    DBusSignatureIter sig_iter;
    gboolean marshalled = FALSE;

    reply = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
    dbus_message_set_destination(reply, sender);
    dbus_message_set_reply_serial(reply, serial);
    dbus_message_set_no_reply(reply, TRUE);

    dbus_message_iter_init_append(reply, &arg_iter);

    if (seed_value_is_undefined (ctx, rval) || g_str_equal(signature, "")) 
      {
        /* We don't want to send anything in these cases so skip the
         * marshalling altogether.
         */
        return reply;
      }
    
    dbus_signature_iter_init(&sig_iter, signature);
    
    if (signature_has_one_element(signature)) 
      {
	marshalled = seed_js_one_value_to_dbus(ctx, rval, &arg_iter, &sig_iter, exception);
      } 
    else 
      {
        if (!seed_value_is_object (ctx, rval))
	  {
	    g_warning("Signature has multiple items but return value is not an array");
	    return reply;
	  }
        marshalled = seed_js_values_to_dbus(ctx, 0, rval, &arg_iter, &sig_iter, exception);
      }

    if (!marshalled) {
        /* replace our planned reply with an error */
        dbus_message_unref(reply);
        if (!dbus_reply_from_exception_and_sender(ctx, sender, serial, &reply, exception))
	  g_warning ("conversion of dbus return value failed but no exception was set?");
    }

    return reply;
}

static DBusMessage*
invoke_js_from_dbus(SeedContext ctx,
                    DBusMessage *method_call,
                    SeedObject this_obj,
                    SeedObject method_obj,
		    SeedException *exception)
{
    DBusMessage *reply;
    int argc;
    SeedValue *argv;
    SeedValue rval;
    DBusMessageIter arg_iter;
    GArray *values;
    const char *signature;

    reply = NULL;

    dbus_message_iter_init(method_call, &arg_iter);

    if (!seed_js_values_from_dbus(ctx, &arg_iter, &values, exception)) 
      {
	if (!dbus_reply_from_exception(ctx, method_call, &reply, exception))
	  g_warning("conversion of dbus method arg failed but no exception was set?");
        return reply;
      }

    argc = values->len;
    argv = (SeedValue *)values->data;

    seed_js_add_dbus_props(ctx, method_call, argv[0], exception);

    rval = seed_object_call (ctx, method_obj, NULL,
		      argc, argv, exception);
    if (!seed_value_is_null (ctx, *exception) &&
	seed_value_is_object (ctx, *exception))
      {
	g_warning("dbus method invocation failed");

        if (!dbus_reply_from_exception(ctx, method_call, &reply, exception))
	  g_warning("dbus method invocation failed but no exception was set?");

        goto out;
      }

    if (dbus_reply_from_exception(ctx, method_call, &reply, exception)) 
      {
	g_warning("Closure invocation succeeded but an exception was set?");
	goto out;
      }

    if (!signature_from_method(ctx,
                               method_obj,
                               &signature, exception)) 
      {
        if (!dbus_reply_from_exception(ctx, method_call, &reply, exception))
	  g_warning("dbus method invocation failed but no exception was set?");
	
        goto out;
      }

    reply = build_reply_from_jsval(ctx,
                                   signature,
                                   dbus_message_get_sender(method_call),
                                   dbus_message_get_serial(method_call),
                                   rval,
				   exception);

 out:
    g_array_free(values, TRUE);

    if (reply)
      g_warning ("Sending %s reply to dbus method %s",
		 dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN ?
		 "normal" : "error",
		 dbus_message_get_member(method_call));
    else
      g_warning ("Failed to create reply to dbus method %s",
		 dbus_message_get_member(method_call));

    return reply;
}

static SeedValue
async_call_callback(SeedContext ctx,
		    SeedObject function,
		    SeedObject this_object,
		    gsize argument_count,
		    const SeedValue arguments[],
		    SeedException *exception)
{
    DBusConnection *connection;
    DBusBusType which_bus;
    DBusMessage *reply;
    const char *sender;
    dbus_uint32_t serial;
    SeedValue prop_value, retval;
    const char *signature;
    gboolean thrown;

    retval = seed_make_undefined (ctx);
    reply = NULL;
    thrown = FALSE;

    prop_value = seed_object_get_property (ctx, function, "_dbusSender");
    sender = seed_value_to_string (ctx, prop_value, exception);
    if (!sender)
      return FALSE;

    prop_value = seed_object_get_property(ctx,
                                     function,
					  "_dbusSerial");
                                
    
    serial = seed_value_to_uint (ctx, prop_value, exception);
    prop_value = seed_object_get_property(ctx,
					  function,
					  "_dbusBusType");
                                
    which_bus = seed_value_to_int(ctx, prop_value, exception);

    /* From now we have enough information to
     * send the exception back to the callee so we'll do so
     */
    prop_value = seed_object_get_property(ctx,
					  function,
					  "_dbusOutSignature");
                                
    signature = seed_value_to_string (ctx, prop_value, exception);
    if (!signature)
        return FALSE;

    if (argument_count != 1) {
      seed_make_exception(ctx, exception, "ArgumentError", 
			  "The callback to async DBus calls takes one argument, "
			  "the return value or array of return values");
        thrown = TRUE;
        goto out;
    }

    reply = build_reply_from_jsval(ctx,
                                   signature,
                                   sender,
                                   serial,
                                   arguments[0],
				   exception);

out:
    if (!reply && thrown) 
      {
	if (!dbus_reply_from_exception_and_sender(ctx, sender, serial, &reply, exception))
	  g_warning("dbus method invocation failed but no exception was set?");
      }

    if (reply) 
      {
        big_dbus_add_bus_weakref(which_bus, &connection);
        if (!connection) 
	  {
            seed_make_exception(ctx, exception, "DBusError", 
				"We were disconnected from the bus before the callback "
				"to some async remote call was called");
            dbus_message_unref(reply);
            big_dbus_remove_bus_weakref(which_bus, &connection);
            return FALSE;
	  }
        dbus_connection_send(connection, reply, NULL);
        big_dbus_remove_bus_weakref(which_bus, &connection);
        dbus_message_unref(reply);
    }

    return retval;
}
