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
