#include <seed.h>

#include "util/dbus.h"

SeedContext ctx;
SeedContextGroup group;

SeedObject namespace_ref;

static gboolean session_bus_weakref_added = FALSE;
static DBusConnection *session_bus = NULL;
static gboolean system_bus_weakref_added = FALSE;
static DBusConnection *system_bus = NULL;

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
			   "dbus.signatureLength expected 1 argument, got %d", argument_count);
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

SeedObject
seed_module_init(SeedEngine * eng)
{
  ctx = eng->context;
  group = eng->group;
  namespace_ref = seed_make_object (eng->context, NULL, NULL);
  
  seed_object_set_property (ctx, namespace_ref, "BUS_SESSION", seed_value_from_int (ctx, DBUS_BUS_SESSION, NULL));
  seed_object_set_property (ctx, namespace_ref, "BUS_SYSTEM", seed_value_from_int (ctx, DBUS_BUS_SYSTEM, NULL));
  seed_object_set_property (ctx, namespace_ref, "BUS_STARTER", seed_value_from_int (ctx, DBUS_BUS_STARTER, NULL));
  
  seed_create_function(ctx, "signatureLength", 
		       (SeedFunctionCallback)seed_js_dbus_signature_length,
		       namespace_ref);
  
  return namespace_ref;
}
