/*
DynamicObject Seed module

this free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.
 
Copyright (C) 2010 Jonatan Liljedahl <lijon@kymatica.com>

--------------------------------------------------------------------------------

Compile with:

gcc -shared -fPIC seed-DynamicObject.c -I/usr/local/include/seed \
    `pkg-config --cflags --libs glib-2.0 gmodule-2.0 gobject-introspection-1.0` \
    -o libseed_DynamicObject.so

Usage:

    o = imports.DynamicObject.create({
        getProperty: function(name) {
            return some_value; // or null to forward to normal props
        },
        SetProperty: function(name, value) {
            do_something(name,value);
            return true; // or false to allow normal props to be set
        },
        deleteProperty: function(name) {
            return true; // or false to forward to normal property deletion
        },
        callAsFunction: function() {
            print("called with args: "+Array.prototype.slice.call(arguments));
        },
        callAsConstructor: function() {
            return {foo:123};
        },
        getPropertyNames: function(){} // not implemented yet...
    });

    o.foobar = 42;      // will call o.set_property('foobar',42)
    print(o.something); // will call o.get_property('something)
    // etc..

'this' inside the callback is set to the calling contexts global object,
not the dynamic object.

You can also create the object first and set the callbacks later:

    o = imports.DynamicObject.create();
    o.callAsFunction = function() { return o.something; }

*/

#include <seed.h>

static SeedClass dynamic_object_class;

static SeedValue
seed_dynamic_object_create (SeedContext ctx,
                            SeedObject function,
                            SeedObject this_object,
                            gsize argument_count,
                            const SeedValue arguments[],
                            SeedException *exception)
{
    SeedObject obj = seed_make_object (ctx, dynamic_object_class, NULL);
    if (argument_count > 0)
        seed_object_set_property (ctx, obj, "__proto__", arguments[0]);

    return (SeedValue)obj;
}

static SeedValue
seed_dynamic_object_get_property (SeedContext ctx,
                                  SeedObject object,
                                  SeedString property_name,
                                  SeedException *exception)
{
    guint len = seed_string_get_maximum_size (property_name);
    gchar *prop = g_alloca (len * sizeof (gchar));
    seed_string_to_utf8_buffer (property_name, prop, len);

    // forward these to ordinary property lookup
    if (!g_strcmp0 (prop, "toString"))
        return NULL;
    if (!g_strcmp0 (prop, "valueOf"))
        return NULL;
    if (!g_strcmp0 (prop, "getProperty"))
        return NULL;
    if (!g_strcmp0 (prop, "setProperty"))
        return NULL;
    if (!g_strcmp0 (prop, "deleteProperty"))
        return NULL;
    if (!g_strcmp0 (prop, "callAsFunction"))
        return NULL;
    if (!g_strcmp0 (prop, "callAsConstructor"))
        return NULL;

    SeedValue handler = seed_object_get_property (ctx, object, "getProperty");
    if (seed_value_is_object (ctx, handler)) {
        SeedValue args[1] = { seed_value_from_string (ctx, prop, exception) };
        SeedValue ret = (SeedValue) seed_object_call (ctx, (SeedObject)handler, NULL, 1, args, exception);
        return seed_value_is_null(ctx,ret)?NULL:ret;
    }

    return NULL;
}

static gboolean
seed_dynamic_object_set_property (SeedContext ctx,
                                  SeedObject object,
                                  SeedString property_name,
                                  SeedValue value,
                                  SeedException *exception)
{
    guint len = seed_string_get_maximum_size (property_name);
    gchar *prop = g_alloca (len * sizeof (gchar));
    seed_string_to_utf8_buffer (property_name, prop, len);

    SeedValue handler = seed_object_get_property (ctx, object, "setProperty");
    if (seed_value_is_object (ctx, handler)) {
        SeedValue args[2] = {
            seed_value_from_string (ctx, prop, exception),
            value};
        SeedValue ret = seed_object_call (ctx, (SeedObject)handler, NULL, 2, args, exception);
        return seed_value_to_boolean (ctx, ret, exception);
    }

    return FALSE;
}

static gboolean 
seed_dynamic_object_delete_property (SeedContext ctx,
                                     SeedObject object,
                                     SeedString property_name,
                                     SeedException *exception)
{
    guint len = seed_string_get_maximum_size (property_name);
    gchar *prop = g_alloca (len * sizeof (gchar));
    seed_string_to_utf8_buffer (property_name, prop, len);

    SeedValue handler = seed_object_get_property (ctx, object, "deleteProperty");
    if (seed_value_is_object (ctx, handler)) {
        SeedValue args[1] = { seed_value_from_string (ctx, prop, exception) };
        SeedValue ret = (SeedValue) seed_object_call (ctx, (SeedObject)handler, NULL, 1, args, exception);
        return seed_value_to_boolean (ctx, ret, exception);
    }
    return FALSE;

}

static SeedValue
seed_dynamic_object_call_as_function (SeedContext ctx,
                                      SeedObject function,
                                      SeedObject this_object,
                                      size_t argument_count,
                                      const SeedValue arguments[],
                                      SeedException *exception)
{
    SeedValue handler = seed_object_get_property (ctx, function, "callAsFunction");
    if (seed_value_is_object (ctx, handler)) {
        return (SeedValue) seed_object_call (ctx, (SeedObject)handler, NULL, argument_count, arguments, exception);
    }
    return NULL;
}

static SeedValue
seed_dynamic_object_call_as_constructor (SeedContext ctx,
                                         SeedObject function,
                                         size_t argument_count,
                                         const SeedValue arguments[],
                                         SeedException *exception)
{
    SeedValue handler = seed_object_get_property (ctx, function, "callAsConstructor");
    if (seed_value_is_object (ctx, handler)) {
        return (SeedValue) seed_object_call (ctx, (SeedObject)handler, NULL, argument_count, arguments, exception);
    }
    return NULL;
}

//static void
//seed_dynamic_object_get_property_names (SeedContext ctx,
//                                        SeedObject object,
//                                        /*wait for API*/ propertyNames)

SeedObject
seed_module_init (SeedEngine * eng)
{
    SeedObject namespace_ref = seed_make_object (eng->context, NULL, NULL);
    seed_class_definition class_def = seed_empty_class;

    class_def.class_name = "DynamicObject";
    class_def.get_property = seed_dynamic_object_get_property;
    class_def.set_property = seed_dynamic_object_set_property;
    class_def.delete_property = seed_dynamic_object_delete_property;
    class_def.call_as_function = seed_dynamic_object_call_as_function;
    class_def.call_as_constructor = seed_dynamic_object_call_as_constructor;
     
    dynamic_object_class = seed_create_class (&class_def);

    seed_create_function(eng->context, "create", (SeedFunctionCallback)seed_dynamic_object_create, namespace_ref);
    return namespace_ref;
}

