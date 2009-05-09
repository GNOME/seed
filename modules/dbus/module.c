#include <seed.h>

#include "util/dbus.h"

SeedContext ctx;
SeedContextGroup group;

SeedObject namespace_ref;
SeedClass dbus_namespace_class;

static gboolean session_bus_weakref_added = FALSE;
static DBusConnection *session_bus = NULL;
static gboolean system_bus_weakref_added = FALSE;
static DBusConnection *system_bus = NULL;

SeedObject bus_proto;

static DBusBusType
get_bus_type_from_object (SeedContext ctx,
			  SeedObject object,
			  SeedException *exception)
{
  SeedValue jsbus_type = seed_object_get_property (ctx, object, "_dbusBusType");
  
  return seed_value_to_int (ctx, jsbus_type, exception);
}

static gboolean
bus_check(SeedContext ctx, DBusBusType bus_type, SeedException *exception)
{
    gboolean bus_weakref_added;
    DBusConnection **bus_connection;

    bus_weakref_added = bus_type == DBUS_BUS_SESSION ? session_bus_weakref_added :
        system_bus_weakref_added;

    bus_connection = bus_type == DBUS_BUS_SESSION ? &session_bus : &system_bus;

    /* This is all done lazily only if a dbus-related method is actually invoked */

    if (!bus_weakref_added) {
        big_dbus_add_bus_weakref(bus_type, bus_connection);
    }

    if (*bus_connection == NULL)
        big_dbus_try_connecting_now(bus_type); /* force a synchronous connection attempt */

    /* Throw exception if connection attempt failed */
    if (*bus_connection == NULL) {
        const char *bus_type_name = bus_type == DBUS_BUS_SESSION ? "session" : "system";
        seed_make_exception(ctx, exception, "BusError",
			    "Not connected to %s message bus", 
			    bus_type_name);
	return FALSE;
    }

    return TRUE;
}

static DBusMessage*
prepare_call(SeedContext ctx,
	     SeedObject obj,
             guint        argc,
             SeedValue *argv,
             DBusBusType  bus_type,
	     SeedException *exception)
{
    DBusMessage *message;
    const char *bus_name;
    const char *path;
    const char *interface;
    const char *method;
    gboolean    auto_start;
    const char *out_signature;
    const char *in_signature;
    DBusMessageIter arg_iter;
    DBusSignatureIter sig_iter;

    if (!bus_check(ctx, bus_type, exception))
        return NULL;

    bus_name = seed_value_to_string (ctx, argv[0], exception);
    if (bus_name == NULL)
      return NULL;

    path = seed_value_to_string (ctx, argv[1], exception);
    if (path == NULL)
      return NULL;

    if (seed_value_is_null(ctx, argv[2])) {
        interface = NULL;
    } else {
      interface = seed_value_to_string (ctx, argv[2], exception);
      if (interface == NULL)
	return NULL; /* exception was set */
    }

    method = seed_value_to_string (ctx, argv[3], exception);
    if (method == NULL)
        return NULL;

    out_signature = seed_value_to_string (ctx, argv[4], exception);
    if (out_signature == NULL)
        return NULL;

    in_signature = seed_value_to_string (ctx, argv[5], exception);
    if (in_signature == NULL)
        return NULL;

    g_assert(bus_name && path && method && in_signature && out_signature);

    auto_start = seed_value_to_boolean(ctx, argv[6], exception);

    /* FIXME should validate the bus_name, path, interface, method really, but
     * we should just not write buggy JS ;-)
     */

    message = dbus_message_new_method_call(bus_name,
                                           path,
                                           interface,
                                           method);
    if (message == NULL)
      {
	seed_make_exception (ctx, exception, "DBusError", "Could not create new method call. (OOM?)");
	return NULL;
      }

    dbus_message_set_auto_start(message, auto_start);

    dbus_message_iter_init_append(message, &arg_iter);

    if (in_signature)
        dbus_signature_iter_init(&sig_iter, in_signature);
    else
        dbus_signature_iter_init(&sig_iter, "a{sv}");

    if (!seed_js_values_to_dbus(ctx, 0, argv[8], &arg_iter, &sig_iter, exception)) {
      //  big_debug(BIG_DEBUG_JS_DBUS, "Failed to marshal call from JS to dbus");
        dbus_message_unref(message);
        return NULL;
    }

    return message;
}

static gboolean
complete_call(SeedContext ctx,
	      SeedValue *retval, 
              DBusMessage *reply,
              DBusError   *derror,
	      SeedException *exception)
{
    DBusMessageIter arg_iter;
    GArray *ret_values;
    int array_length;

    if (dbus_error_is_set(derror))
      {
	//        big_debug(BIG_DEBUG_JS_DBUS,
	//          "Error sending call: %s: %s",
	//          derror->name, derror->message);
	seed_make_exception(ctx, exception, "DBusError",
			    "DBus error: %s: %s",
			    derror->name,
			    derror->message);
	dbus_error_free(derror);
	return FALSE;
    }

    if (reply == NULL) 
      {
	// big_debug(BIG_DEBUG_JS_DBUS,
	//        "No reply received to call");
	return FALSE;
      }

    if (dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_ERROR) 
      {
	seed_make_exception (ctx, exception, "DBusError",
			     "DBus error: %s: %s",
			     derror->name,
			     derror->message);
        dbus_error_free(derror);

	return FALSE;
    }

    dbus_message_iter_init(reply, &arg_iter);
    if (!seed_js_values_from_dbus(ctx, &arg_iter, &ret_values, exception)) 
      {
	//        big_debug(BIG_DEBUG_JS_DBUS, "Failed to marshal dbus call reply back to JS");
	return FALSE;
      }

    g_assert(ret_values != NULL);
    // TODO: I AM HERE

    array_length = ret_values->len;
    if (array_length == 1) {
        /* If the array only has one element return that element alone */
      *retval = g_array_index (ret_values, SeedValue, 0);
    } else {
        /* Otherwise return an array with all the return values. The
         * funny assignment is to avoid creating a temporary JSObject
         * we'd need to root
         */
      *retval = seed_make_array (ctx, ret_values->data, array_length, exception);
    }

    g_array_free (ret_values, TRUE);    

    seed_js_add_dbus_props(ctx, reply, *retval);

    return TRUE;
}

			  

static SeedValue
seed_js_dbus_get_machine_id (SeedContext ctx,
			     SeedObject object,
			     SeedString property_name,
			     SeedException *exception)
{
  SeedValue ret;
  gchar *id;
  
  id = dbus_get_local_machine_id ();
  ret = seed_value_from_string (ctx, id, exception);
  dbus_free (id);
  
  return ret;
}

static SeedValue
seed_js_dbus_signature_length (SeedContext ctx,
			       SeedObject function,
			       SeedObject this_object,
			       size_t argument_count,
			       const SeedValue arguments[],
			       SeedException *exception)
{
  const gchar *signature;
  DBusSignatureIter iter;
  gint length = 0;
  
  if (argument_count < 1) 
    {
      seed_make_exception (ctx, exception, "ArgumentError", 
			   "dbus.signatureLength expected 1 argument, got %zd", argument_count);
      return seed_make_null (ctx);
    }
  
  signature = seed_value_to_string (ctx, arguments[0], exception);
  
  if (!dbus_signature_validate (signature, NULL)) {
    seed_make_exception (ctx, exception, "ArgumentError",
			 "Invalid signature");
    return seed_make_null (ctx);
  }
  
  if (*signature == '\0')
    return seed_value_from_int (ctx, 0, exception);
  
  dbus_signature_iter_init (&iter, signature);
  do {
    length++;
  } while (dbus_signature_iter_next (&iter));
  
  return seed_value_from_int (ctx, length, exception);
}

seed_static_function dbus_funcs[] = {
  {"signatureLength", seed_js_dbus_signature_length, 0},
  {0, 0, 0}
};

seed_static_value dbus_values[] = {
  {"localMachineID", seed_js_dbus_get_machine_id, 0, 0},
  {0, 0, 0, 0}
};

static void
seed_define_bus_proto (SeedContext ctx)
{
  bus_proto = seed_make_object (ctx, NULL, NULL);
    
  seed_value_protect (ctx, bus_proto);
}

SeedObject
seed_module_init(SeedEngine * eng)
{
  seed_class_definition dbus_namespace_class_def = seed_empty_class;

  ctx = eng->context;
  group = eng->group;

  dbus_namespace_class_def.class_name = "dbusnative";
  dbus_namespace_class_def.static_functions = dbus_funcs;
  dbus_namespace_class_def.static_values = dbus_values;
  
  dbus_namespace_class = seed_create_class (&dbus_namespace_class_def);

  namespace_ref = seed_make_object (eng->context, dbus_namespace_class, NULL);
  
  seed_object_set_property (ctx, namespace_ref, "BUS_SESSION", seed_value_from_int (ctx, DBUS_BUS_SESSION, NULL));
  seed_object_set_property (ctx, namespace_ref, "BUS_SYSTEM", seed_value_from_int (ctx, DBUS_BUS_SYSTEM, NULL));
  seed_object_set_property (ctx, namespace_ref, "BUS_STARTER", seed_value_from_int (ctx, DBUS_BUS_STARTER, NULL));
  
  seed_create_function(ctx, "signatureLength", 
		       (SeedFunctionCallback)seed_js_dbus_signature_length,
		       namespace_ref);
  
  seed_define_bus_proto (ctx);
  
  return namespace_ref;
}
