#include <seed.h>

#include "util/dbus.h"
#include "dbus-values.h"

#define DBUS_CONNECTION_FROM_TYPE(type) ((type) == DBUS_BUS_SESSION ? session_bus : system_bus)

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
             const SeedValue *argv,
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

    seed_js_add_dbus_props(ctx, reply, retval, exception);

    return TRUE;
}

static void
pending_notify(DBusPendingCall *pending,
               void            *user_data)
{
  SeedException exception;
  SeedContext ctx;
  GClosure *closure;
  SeedValue argv[2];
  DBusMessage *reply;
  DBusError derror;
  
  closure = user_data;

//    big_debug(BIG_DEBUG_JS_DBUS,
    //            "Notified of reply to async call closure %p context %p",
    //        closure, context);

//    if (context == NULL) {
    //      big_debug(BIG_DEBUG_JS_DBUS,
    //            "Closure destroyed before we could complete pending call");
    //  return;
//    }

    /* reply may be NULL if none received? I think it may never be if
     * we've already been notified, but be safe here.
     */
  
  ctx = seed_context_create (group, NULL);
  seed_prepare_global_context (ctx);
  reply = dbus_pending_call_steal_reply(pending);
  
  dbus_error_init(&derror);
  /* argv[0] will be the return value if any, argv[1] we fill with exception if any */
  complete_call(ctx, &argv[0], reply, &derror, &exception);
  g_assert(!dbus_error_is_set(&derror)); /* not supposed to be left set by complete_call() */
  
  if (reply)
    dbus_message_unref(reply);
  
  seed_closure_invoke_with_context(ctx, closure, &argv[0], 2, &exception);
  seed_context_unref (ctx);
  // TODO: Do something with exception
}

static void
pending_free_closure(void *data)
{
    GClosure *closure;

    closure = data;

    g_closure_invalidate(data);
    g_closure_unref(data);
}


static SeedValue
seed_js_dbus_call_async(SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			size_t argument_count,
			const SeedValue arguments[],
			SeedException *exception)
{
    GClosure *closure;
    DBusMessage *message;
    DBusPendingCall *pending;
    DBusConnection  *bus_connection;
    DBusBusType bus_type;
    int timeout;

    if (argument_count < 10) {
      seed_make_exception (ctx, exception, "ArgmuentError",
			   "Not enough args, need bus name, object path, interface, method, out signature, in signature, autostart flag, timeout limit, args, and callback");
      return seed_make_null (ctx);
    }

    if (!seed_value_is_object (ctx, arguments[9])  || !seed_value_is_function (ctx, arguments[9]))
      {
	seed_make_exception(ctx, exception, "ArgumentError", "arg 10 must be a callback to invoke when call completes");
	return FALSE;
      }

    timeout = seed_value_to_int (ctx, arguments[7], exception);

    bus_type = get_bus_type_from_object (ctx, this_object, exception);

    message = prepare_call(ctx, this_object, argument_count, arguments, bus_type, exception);

    if (message == NULL)
      return seed_make_null (ctx);

    bus_connection = DBUS_CONNECTION_FROM_TYPE(bus_type);

    pending = NULL;
    if (!dbus_connection_send_with_reply(bus_connection, message, &pending, timeout) ||
        pending == NULL) 
      {
	//        big_debug(BIG_DEBUG_JS_DBUS, "Failed to send async dbus message");
	seed_make_exception(ctx, exception, "DBusError", "Failed to send dbus message");
        dbus_message_unref(message);
        return seed_make_null (ctx);
      }

    g_assert(pending != NULL);

    dbus_message_unref(message);

    /* We cheat a bit here and use a closure to store a JavaScript function
     * and deal with the GC root and other issues, even though we
     * won't ever marshal via GValue
     */
    closure = seed_make_gclosure (ctx, arguments[9], NULL);
    if (closure == NULL) {
        dbus_pending_call_unref(pending);
	return seed_make_null (ctx);
    }

    g_closure_ref(closure);
    g_closure_sink(closure);
    dbus_pending_call_set_notify(pending, pending_notify, closure,
                                 pending_free_closure);

    dbus_pending_call_unref(pending); /* DBusConnection should still hold a ref until it's completed */

    return seed_value_from_boolean (ctx, TRUE, exception);
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

static void
fill_with_null_or_string(SeedContext ctx, const char **string_p, SeedValue value, SeedException *exception)
{
  if (seed_value_is_null (ctx, value))
        *string_p = NULL;
    else 
      *string_p = seed_value_to_string (ctx, value, exception);
}

typedef struct {
    int refcount;
    DBusBusType bus_type;
    int connection_id;
    GClosure *closure;
} SignalHandler;
/* allow removal by passing in the callable
 * FIXME don't think we ever end up using this,
 * could get rid of it, it predates having an ID
 * to remove by
 */
static GHashTable *signal_handlers_by_callable = NULL;

static void signal_on_closure_invalidated (void          *data,
                                           GClosure      *closure);
static void signal_handler_ref            (SignalHandler *handler);
static void signal_handler_unref          (SignalHandler *handler);

static SignalHandler*
signal_handler_new(SeedContext ctx,
                   SeedValue      callable,
		   SeedException *exception)
{
    SignalHandler *handler;

    if (signal_handlers_by_callable &&
        g_hash_table_lookup(signal_handlers_by_callable,
                            callable) != NULL) 
      {
        /* To fix this, get rid of signal_handlers_by_callable
         * and just require removal by id. Not sure we ever use
         * removal by callable anyway.
         */
	seed_make_exception(ctx, exception, "ArgumentError",
			    "For now, same callback cannot be the handler for two dbus signal connections");
        return NULL;
      }

    handler = g_slice_new0(SignalHandler);
    handler->refcount = 1;

    /* We cheat a bit here and use a closure to store a JavaScript function
     * and deal with the GC root and other issues, even though we
     * won't ever marshal via GValue
     */
    handler->closure = seed_make_gclosure(ctx,
					  callable,
					  NULL);
    if (handler->closure == NULL) {
        g_free(handler);
        return NULL;
    }

    g_closure_ref(handler->closure);
    g_closure_sink(handler->closure);

    g_closure_add_invalidate_notifier(handler->closure, handler,
                                      signal_on_closure_invalidated);

    if (!signal_handlers_by_callable) {
        signal_handlers_by_callable =
            g_hash_table_new_full(g_direct_hash,
                                  g_direct_equal,
                                  NULL,
                                  NULL);
    }

    /* We keep a weak reference on the closure in a table indexed
     * by the object, so we can retrieve it when removing the signal
     * watch. The signal_handlers_by_callable owns one ref to the SignalHandler.
     */
    signal_handler_ref(handler);
    g_hash_table_replace(signal_handlers_by_callable,
                         callable,
                         handler);

    return handler;
}

static void
signal_handler_ref(SignalHandler *handler)
{
    g_assert(handler->refcount > 0);
    handler->refcount += 1;
}

static void
signal_handler_dispose(SignalHandler *handler)
{
    g_assert(handler->refcount > 0);

    signal_handler_ref(handler);

    if (handler->closure) {
        /* invalidating closure could dispose
         * re-entrantly, so set handler->closure
         * NULL before we invalidate
         */
        GClosure *closure = handler->closure;
        handler->closure = NULL;

        g_hash_table_remove(signal_handlers_by_callable,
                            seed_closure_get_callable(closure));
        if (g_hash_table_size(signal_handlers_by_callable) == 0) {
            g_hash_table_destroy(signal_handlers_by_callable);
            signal_handlers_by_callable = NULL;
        }
        /* the hash table owned 1 ref */
        signal_handler_unref(handler);

        g_closure_invalidate(closure);
        g_closure_unref(closure);
    }

    /* remove signal if it hasn't been */
    if (handler->connection_id != 0) {
        int id = handler->connection_id;
        handler->connection_id = 0;

        /* this should clear another ref off the
         * handler by calling signal_on_watch_removed
         */
        big_dbus_unwatch_signal_by_id(handler->bus_type,
                                      id);
    }

    signal_handler_unref(handler);
}

static void
signal_handler_unref(SignalHandler *handler)
{
    g_assert(handler->refcount > 0);

    if (handler->refcount == 1) {
        signal_handler_dispose(handler);
    }

    handler->refcount -= 1;
    if (handler->refcount == 0) {
        g_assert(handler->closure == NULL);
        g_assert(handler->connection_id == 0);
        g_slice_free(SignalHandler, handler);
    }
}

static void
signal_on_watch_removed(void *data)
{
    SignalHandler *handler = data;

    handler->connection_id = 0; /* don't re-remove it */

    /* The watch owns a ref; removing it
     * also forces dispose, which invalidates
     * the closure if that hasn't been done.
     */
    signal_handler_dispose(handler);
    signal_handler_unref(handler);
}

static void
signal_on_closure_invalidated(void     *data,
                              GClosure *closure)
{
    SignalHandler *handler;

    handler = data;

    /* this removes the watch if it has not been */
    signal_handler_dispose(handler);
}

static void
signal_handler_callback(DBusConnection *connection,
                        DBusMessage    *message,
                        void           *data)
{
  SeedContext ctx;
  SignalHandler *handler;
  SeedValue ret_val;
  DBusMessageIter arg_iter;
  GArray *arguments;
  SeedException exception;

  //    big_debug(BIG_DEBUG_JS_DBUS,
  //          "Signal handler called");

    handler = data;

    if (handler->closure == NULL) {
      //        big_debug(BIG_DEBUG_JS_DBUS, "dbus signal handler invalidated, ignoring");
        return;
    }

    ctx = seed_context_create (group, NULL);
    seed_prepare_global_context (ctx);

    dbus_message_iter_init(message, &arg_iter);
    if (!seed_js_values_from_dbus(ctx, &arg_iter, &arguments, &exception)) {
      //        big_debug(BIG_DEBUG_JS_DBUS, "Failed to marshal dbus signal to JS");
        return;
    }

    signal_handler_ref(handler); /* for safety, in case handler removes itself */

    g_assert(arguments != NULL);

    //    big_debug(BIG_DEBUG_JS_DBUS,
    //        "Invoking closure on signal received, %d args",
    //        gjs_rooted_array_get_length(context, arguments));
    ret_val = seed_closure_invoke_with_context(ctx, handler->closure,
					       (SeedValue *)arguments->data,
					       arguments->len,
					       &exception);

    g_array_free (arguments, TRUE);

    signal_handler_unref(handler); /* for safety */
}

static SeedValue
seed_js_dbus_watch_signal (SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   size_t argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
    const char *bus_name;
    const char *object_path;
    const char *iface;
    const char *signal;
    SignalHandler *handler;
    int id;
    DBusBusType bus_type;

    if (argument_count < 5) 
      {
	seed_make_exception(ctx, exception, "ArgumentError", "Not enough args, need bus name, object path, interface, signal and callback");
	return seed_make_null (ctx);
      }

    if (!seed_value_is_object (ctx, arguments[4]) || !seed_value_is_function (ctx, arguments[4]))
      {
	seed_make_exception(ctx, exception, "ArgumentError", "arg 5 must be a callback to invoke when call completes");
	return seed_make_null (ctx);
      }

    fill_with_null_or_string(ctx, &bus_name, arguments[0], exception);
    fill_with_null_or_string(ctx, &object_path, arguments[1], exception);
    fill_with_null_or_string(ctx, &iface, arguments[2], exception);
    fill_with_null_or_string(ctx, &signal, arguments[3], exception);

    bus_type = get_bus_type_from_object (ctx, this_object, exception);

    handler = signal_handler_new(ctx, arguments[4], exception);
    if (handler == NULL)
      return seed_make_null (ctx);

    id = big_dbus_watch_signal(bus_type,
                               bus_name,
                               object_path,
                               iface,
                               signal,
                               signal_handler_callback,
                               handler,
                               signal_on_watch_removed);
    handler->bus_type = bus_type;
    handler->connection_id = id;

    /* signal_on_watch_removed() takes ownership of our
     * ref to the SignalHandler
     */

    return seed_value_from_int (ctx, id, exception);
}

/* Args are handler id */
static SeedValue
seed_js_dbus_unwatch_signal_by_id(SeedContext ctx,
				 SeedObject function,
				 SeedObject this_object,
				 size_t argument_count,
				 const SeedValue arguments[],
				 SeedException *exception)
{
    int id;
    DBusBusType bus_type;

    if (argument_count < 1) 
      {
	seed_make_exception (ctx, exception, "ArgumentError", "Not enough args, need handler id");
	return seed_make_null (ctx);
      }

    bus_type = get_bus_type_from_object (ctx, this_object, exception);
    id = seed_value_to_int (ctx, arguments[0], exception);

    big_dbus_unwatch_signal_by_id(bus_type,
                                  id);
    return seed_make_undefined (ctx);
}

static SeedValue
seed_js_dbus_unwatch_signal(SeedContext ctx,
			   SeedObject function,
			   SeedObject this_object,
			   size_t argument_count,
			   const SeedValue arguments[],
			   SeedException *exception)
{
    const char *bus_name;
    const char *object_path;
    const char *iface;
    const char *signal;
    SignalHandler *handler;
    DBusBusType bus_type;

    if (argument_count < 5) 
      {
	seed_make_exception (ctx, exception, "ArgumentError",  "Not enough args, need bus name, object path, interface, signal and callback");
	return seed_make_null (ctx);
      }

    bus_type = get_bus_type_from_object (ctx, this_object, exception);

    if (!seed_value_is_object (ctx, arguments[4]) || !seed_value_is_function (ctx, arguments[4]))
      {
        seed_make_exception (ctx, exception, "ArgumentError", "arg 5 must be a callback to invoke when call completes");
	return seed_make_null (ctx);
      }

    fill_with_null_or_string(ctx, &bus_name, arguments[0], exception);
    fill_with_null_or_string(ctx, &object_path, arguments[1], exception);
    fill_with_null_or_string(ctx, &iface, arguments[2], exception);
    fill_with_null_or_string(ctx, &signal, arguments[3], exception);
    
    /* we don't complain if the signal seems to have been already removed
     * or to never have been watched, to match g_signal_handler_disconnect
     */
    if (!signal_handlers_by_callable)
      return seed_make_undefined (ctx);

    handler = g_hash_table_lookup(signal_handlers_by_callable, arguments[4]);

    if (!handler)
      return seed_make_undefined (ctx);

    /* This should dispose the handler which should in turn
     * remove it from the handler table
     */
    big_dbus_unwatch_signal(bus_type,
                            bus_name,
                            object_path,
                            iface,
                            signal,
                            signal_handler_callback,
                            handler);

    g_assert(g_hash_table_lookup(signal_handlers_by_callable,
                                 arguments[4]) == NULL);

    return seed_make_undefined (ctx);
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
