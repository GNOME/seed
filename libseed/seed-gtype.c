/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * seed-gtype.c
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 *
 * libseed is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libseed is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "seed-private.h"
#include <sys/mman.h>

JSClassRef seed_gtype_class;

/* From pygobject */
static ffi_type *
g_value_to_ffi_type (const GValue *gvalue, gpointer *value)
{
    ffi_type *rettype = NULL;
    GType type = g_type_fundamental (G_VALUE_TYPE (gvalue));
    g_assert (type != G_TYPE_INVALID);

    switch (type) {
    case G_TYPE_BOOLEAN:
    case G_TYPE_CHAR:
    case G_TYPE_INT:
	rettype = &ffi_type_sint;
	*value = (gpointer)&(gvalue->data[0].v_int);
	break;
    case G_TYPE_UCHAR:
    case G_TYPE_UINT:
	rettype = &ffi_type_uint;
	*value = (gpointer)&(gvalue->data[0].v_uint);
	break;
    case G_TYPE_STRING:
    case G_TYPE_OBJECT:
    case G_TYPE_BOXED:
    case G_TYPE_POINTER:
	rettype = &ffi_type_pointer;
	*value = (gpointer)&(gvalue->data[0].v_pointer);
	break;
    case G_TYPE_FLOAT:
	rettype = &ffi_type_float;
	*value = (gpointer)&(gvalue->data[0].v_float);
	break;
    case G_TYPE_DOUBLE:
	rettype = &ffi_type_double;
	*value = (gpointer)&(gvalue->data[0].v_double);
	break;
    case G_TYPE_LONG:
	rettype = &ffi_type_slong;
	*value = (gpointer)&(gvalue->data[0].v_long);
	break;
    case G_TYPE_ULONG:
	rettype = &ffi_type_ulong;
	*value = (gpointer)&(gvalue->data[0].v_ulong);
	break;
    case G_TYPE_INT64:
	rettype = &ffi_type_sint64;
	*value = (gpointer)&(gvalue->data[0].v_int64);
	break;
    case G_TYPE_UINT64:
	rettype = &ffi_type_uint64;
	*value = (gpointer)&(gvalue->data[0].v_uint64);
	break;
    default:
	rettype = &ffi_type_pointer;
	*value = NULL;
	g_warning ("Unsupported fundamental type: %s", g_type_name (type));
	break;
    }
    return rettype;
}

/* From pygobject */
static void
g_value_from_ffi_type (GValue *gvalue, gpointer *value)
{
    switch (g_type_fundamental (G_VALUE_TYPE (gvalue))) {
    case G_TYPE_INT:
	g_value_set_int (gvalue, *(gint*)value);
	break;
    case G_TYPE_FLOAT:
	g_value_set_float (gvalue, *(gfloat*)value);
	break;
    case G_TYPE_DOUBLE:
	g_value_set_double (gvalue, *(gdouble*)value);
	break;
    case G_TYPE_BOOLEAN:
	g_value_set_boolean (gvalue, *(gboolean*)value);
	break;
    case G_TYPE_STRING:
	g_value_set_string (gvalue, *(gchar**)value);
	break;
    case G_TYPE_CHAR:
	g_value_set_char (gvalue, *(gchar*)value);
	break;
    case G_TYPE_UCHAR:
	g_value_set_uchar (gvalue, *(guchar*)value);
	break;
    case G_TYPE_UINT:
	g_value_set_uint (gvalue, *(guint*)value);
	break;
    case G_TYPE_POINTER:
	g_value_set_pointer (gvalue, *(gpointer*)value);
	break;
    case G_TYPE_LONG:
	g_value_set_long (gvalue, *(glong*)value);
	break;
    case G_TYPE_ULONG:
	g_value_set_ulong (gvalue, *(gulong*)value);
	break;
    case G_TYPE_INT64:
	g_value_set_int64 (gvalue, *(gint64*)value);
	break;
    case G_TYPE_UINT64:
	g_value_set_uint64 (gvalue, *(guint64*)value);
	break;
    case G_TYPE_BOXED:
	g_value_set_boxed (gvalue, *(gpointer*)value);
	break;
    default:
	g_warning ("Unsupported fundamental type: %s",
		   g_type_name (g_type_fundamental (G_VALUE_TYPE (gvalue))));
    }

}

/* from pygobject */
void
g_cclosure_marshal_generic_ffi (GClosure *closure,
				GValue *return_gvalue,
				guint n_param_values,
				const GValue *param_values,
				gpointer invocation_hint,
				gpointer marshal_data)
{
    ffi_type *rtype;
    void *rvalue;
    int n_args;
    ffi_type **atypes;
    void **args;
    int i;
    ffi_cif cif;
    GCClosure *cc = (GCClosure*) closure;

    if (return_gvalue && G_VALUE_TYPE (return_gvalue)) 
    {
	rtype = g_value_to_ffi_type (return_gvalue, &rvalue);
    }
    else 
    {
	rtype = &ffi_type_void;
    }

    rvalue = g_alloca (MAX (rtype->size, sizeof (ffi_arg)));
  
    n_args = n_param_values + 1;
    atypes = g_alloca (sizeof (ffi_type *) * n_args);
    args =  g_alloca (sizeof (gpointer) * n_args);

    if (G_CCLOSURE_SWAP_DATA (closure))
    {
	atypes[n_args-1] = g_value_to_ffi_type (param_values + 0,  
						&args[n_args-1]);
	atypes[0] = &ffi_type_pointer;
	args[0] = &closure->data;
    }
    else
    {
	atypes[0] = g_value_to_ffi_type (param_values + 0, &args[0]);
	atypes[n_args-1] = &ffi_type_pointer;
	args[n_args-1] = &closure->data;
    }

    for (i = 1; i < n_args - 1; i++)
	atypes[i] = g_value_to_ffi_type (param_values + i, &args[i]);

    if (ffi_prep_cif (&cif, FFI_DEFAULT_ABI, n_args, rtype, atypes) != FFI_OK)
	return;

    ffi_call (&cif, marshal_data ? marshal_data : cc->callback, rvalue, args);

    if (return_gvalue && G_VALUE_TYPE (return_gvalue))
	g_value_from_ffi_type (return_gvalue, rvalue);
}

static JSValueRef
seed_property_method_invoked(JSContextRef ctx,
			    JSObjectRef function,
			    JSObjectRef thisObject,
			    size_t argumentCount,
			    const JSValueRef arguments[],
			    JSValueRef * exception)
{
    GParamSpec * spec;
    GObjectClass * class;
    int property_count;
    JSValueRef newcount, oldcount;

    if (argumentCount != 1)
    {
	gchar * mes = g_strdup_printf("Property installation expected 1 argument",
				      " got %d \n", argumentCount);
	seed_make_exception(exception, "ArgumentError", mes);
	g_free(mes);
	
	return JSValueMakeNull(eng->context);
    }
    
    if (JSValueIsNull(eng->context, arguments[0]) || 
	// Might need to check if JSValueIsObject? Who knows with WebKit.
	!JSValueIsObjectOfClass(eng->context, arguments[0], seed_struct_class))
    {
	seed_make_exception(exception, "ArgumentError", "Property installation expected a "
			    "GParamSpec as argument");
	return JSValueMakeNull(eng->context);
    }
    
    spec = (GParamSpec *)seed_struct_get_pointer(arguments[0]);
    
    oldcount = seed_object_get_property(thisObject, "property_count");
    property_count = seed_value_to_int(oldcount, exception);
    

    class = seed_struct_get_pointer(thisObject);
    g_object_class_install_property(class, property_count, spec);
    
    newcount = seed_value_from_int(property_count+1, exception);
    seed_object_set_property(thisObject, 
			     "property_count",
			     newcount);
    
    return oldcount;
}

static JSValueRef
seed_gsignal_method_invoked(JSContextRef ctx,
			    JSObjectRef function,
			    JSObjectRef thisObject,
			    size_t argumentCount,
			    const JSValueRef arguments[],
			    JSValueRef * exception)
{
    // TODO: class_closure, and accumlator. Not useful until we have structs.
    JSValueRef jsname, jstype, jsflags, jsreturn_type, jsparams, ret;
    GType itype, return_type;
    guint n_params = 0;
    GType * param_types = 0;
    gchar * name;
    guint signal_id;
    GSignalFlags flags;

    /* Sanity check */
    if (argumentCount != 1)
    {
	gchar * mes = g_strdup_printf("Signal constructor expected 1 argument",
				      " got %d \n", argumentCount);
	seed_make_exception(exception, "ArgumentError", mes);
	g_free(mes);
	return (JSObjectRef)JSValueMakeNull(eng->context);
    }
    if (JSValueIsNull(eng->context, arguments[0]) || 
	!JSValueIsObject(eng->context, arguments[0]))
    {
	seed_make_exception(exception, "ArgumentError",
			    "Signal constructor expected object"
			    " as first argument");
 	return (JSObjectRef)JSValueMakeNull(eng->context);
    }


    /* Signal name */
    jsname = seed_object_get_property((JSObjectRef)arguments[0], "name");
    /* seed_value_to_string can handle non strings, however the kind
     * of strings we want as a signal name are rather small, so make sure
     * we have an actual string */
    if (JSValueIsNull(eng->context, jsname) || 
	!JSValueIsString(eng->context, jsname))
    {
	seed_make_exception(exception, "ArgumentError",
			    "Signal definition needs name property");
	return (JSObjectRef)JSValueMakeNull(eng->context);
    }
    name = seed_value_to_string(jsname, exception);
    
    /* Type to install on. Comes from class. */
    jstype = seed_object_get_property(thisObject, "type");
    itype = seed_value_to_int(jstype, exception);
    
    /* Signal flags */
    jsflags = seed_object_get_property((JSObjectRef)arguments[0], "flags");
    if (JSValueIsNull(eng->context, jsflags) || 
	!JSValueIsNumber(eng->context, jsflags))
	flags = G_SIGNAL_RUN_LAST;
    else
	flags = seed_value_to_long(jsflags, exception);
    
    
    /* Return type */
    jsreturn_type = seed_object_get_property((JSObjectRef)arguments[0],
					    "return_type");
    if (JSValueIsNull(eng->context, jsreturn_type) || 
	!JSValueIsNumber(eng->context, jsreturn_type))
	return_type = G_TYPE_NONE;
    else
	return_type = seed_value_to_int(jsreturn_type, exception);

    /* Number of params and types */
    jsparams = seed_object_get_property((JSObjectRef)arguments[0],
				       "parameters");
    if (!JSValueIsNull(eng->context, jsparams) &&
	JSValueIsObject(eng->context, jsparams))
    {
	n_params = seed_value_to_int
	    (seed_object_get_property((JSObjectRef)jsparams, "length"), 
	     exception);
	if (n_params > 0)
	{
	    guint i;
	    
	    param_types = g_new0(GType, n_params);
	    for (i = 0; i < n_params; i++)
	    {
		JSValueRef ptype = 
		    JSObjectGetPropertyAtIndex(eng->context,
					       (JSObjectRef)jsparams, 
					       i,
					       exception);
		
		param_types[i] = seed_value_to_int(ptype, exception);
	    }
	}
    }   
    
    signal_id = g_signal_newv(name, itype,
			      flags, 0, 0, 0, 
			      g_cclosure_marshal_generic_ffi,
			      return_type,
			      n_params,
			      param_types);
    
    g_free(name);
    g_free(param_types);
    
    return (JSValueRef)seed_value_from_uint(signal_id, exception);
}



static void
seed_handle_class_init_closure(ffi_cif * cif,
			       void *result, void **args, void *userdata)
{
    JSObjectRef function = (JSObjectRef) userdata;
    JSValueRef jsargs[2];
    GType type;
    JSValueRef exception = 0;

    type = (GType) JSObjectGetPrivate(*(JSObjectRef *) args[1]);
    jsargs[0] = seed_make_struct(*(gpointer *) args[0], 0);
    jsargs[1] = seed_gobject_get_prototype_for_gtype(type);

    // TODO: 
    // Should probably have a custom type for class, and have it auto convert.
    seed_object_set_property((JSObjectRef)jsargs[0], 
			    "type", seed_value_from_int(type, 0));
    seed_object_set_property((JSObjectRef)jsargs[0],
			     "property_count", seed_value_from_int(1, 0));
    seed_create_function("install_signal",
			 &seed_gsignal_method_invoked,
			 (JSObjectRef)jsargs[0]);
    seed_create_function("install_property",
			 &seed_property_method_invoked,
			 (JSObjectRef)jsargs[0]);

    JSObjectCallAsFunction(eng->context, function, 0, 2, jsargs, 0);
    if (exception)
    {
	gchar *mes = seed_exception_to_string(exception);
	g_warning("Exception in class init closure. %s \n", mes, 0);
    }
}

static void
seed_handle_instance_init_closure(ffi_cif * cif,
				  void *result, void **args, void *userdata)
{
    JSObjectRef function = (JSObjectRef) userdata;
    JSValueRef jsargs;
    JSValueRef exception = 0;
    JSObjectRef this_object;

    jsargs = seed_make_struct(*(gpointer *) args[1], 0);
    this_object =
	(JSObjectRef) seed_value_from_object(*(GObject **) args[0], 0);

    JSObjectCallAsFunction(eng->context, function, this_object, 1, &jsargs,
			   &exception);
    if (exception)
    {
	gchar *mes = seed_exception_to_string(exception);
	g_warning("Exception in instance init closure. %s \n", mes, 0);
    }

}

static void
seed_handle_set_property_closure(ffi_cif * cif,
				  void *result, void **args, void *userdata)
{
    JSObjectRef function = (JSObjectRef) userdata;
    JSValueRef jsargs[2];
    JSValueRef exception = 0;
    JSObjectRef this_object;
    GParamSpec * spec;

    this_object =
	(JSObjectRef) seed_value_from_object(*(GObject **) args[0], &exception);
    spec = *(GParamSpec **)args[3];
    
    jsargs[0] = seed_value_from_string(spec->name, &exception);
    jsargs[1] = seed_value_from_gvalue(*(GValue **) args[2], &exception);

    JSObjectCallAsFunction(eng->context, function, this_object, 2, jsargs,
			   &exception);
    if (exception)
    {
	gchar *mes = seed_exception_to_string(exception);
	g_warning("Exception in instance init closure. %s \n", mes, 0);
    }

}

static ffi_closure *seed_make_class_init_closure(JSObjectRef function)
{
    ffi_cif *cif;
    ffi_closure *closure;
    ffi_type **arg_types;;
    ffi_arg result;
    ffi_status status;

    JSValueProtect(eng->context, function);

    cif = g_new0(ffi_cif, 1);
    arg_types = g_new0(ffi_type *, 3);

    arg_types[0] = &ffi_type_pointer;
    arg_types[1] = &ffi_type_uint;
    arg_types[2] = 0;

    closure = mmap(0, sizeof(ffi_closure), PROT_READ | PROT_WRITE |
		   PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);

    ffi_prep_cif(cif, FFI_DEFAULT_ABI, 2, &ffi_type_void, arg_types);
    ffi_prep_closure(closure, cif, seed_handle_class_init_closure, function);
    return closure;
}

static ffi_closure *seed_make_instance_init_closure(JSObjectRef function)
{
    ffi_cif *cif;
    ffi_closure *closure;
    ffi_type **arg_types;;
    ffi_arg result;
    ffi_status status;

    JSValueProtect(eng->context, function);

    cif = g_new0(ffi_cif, 1);
    arg_types = g_new0(ffi_type *, 3);

    arg_types[0] = &ffi_type_pointer;
    arg_types[1] = &ffi_type_pointer;
    arg_types[2] = 0;

    closure = mmap(0, sizeof(ffi_closure), PROT_READ | PROT_WRITE |
		   PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);

    ffi_prep_cif(cif, FFI_DEFAULT_ABI, 2, &ffi_type_void, arg_types);
    ffi_prep_closure(closure, cif, seed_handle_instance_init_closure, function);
    return closure;
}

static ffi_closure *seed_make_set_property_closure(JSObjectRef function)
{
    ffi_cif *cif;
    ffi_closure *closure;
    ffi_type **arg_types;;
    ffi_arg result;
    ffi_status status;

    JSValueProtect(eng->context, function);

    cif = g_new0(ffi_cif, 1);
    arg_types = g_new0(ffi_type *, 5);

    arg_types[0] = &ffi_type_pointer;
    arg_types[1] = &ffi_type_uint;
    arg_types[2] = &ffi_type_pointer;
    arg_types[3] = &ffi_type_pointer;
    arg_types[4] = 0;

    closure = mmap(0, sizeof(ffi_closure), PROT_READ | PROT_WRITE |
		   PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);

    ffi_prep_cif(cif, FFI_DEFAULT_ABI, 4, &ffi_type_void, arg_types);
    ffi_prep_closure(closure, cif, seed_handle_set_property_closure, function);
    return closure;
}

static JSObjectRef
seed_gtype_constructor_invoked(JSContextRef ctx,
			       JSObjectRef constructor,
			       size_t argumentCount,
			       const JSValueRef arguments[],
			       JSValueRef * exception)
{
    JSValueRef class_init, instance_init, name, parent_ref, set_property_ref;
    GType parent_type, new_type;
    gchar *new_name;
    GTypeInfo type_info = {
	0,
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) NULL,
	(GClassFinalizeFunc) NULL,
	NULL,
	0,
	0,
	NULL
    };
    ffi_closure *init_closure = 0;
    ffi_closure *instance_init_closure = 0;

    GTypeQuery query;
    JSObjectRef constructor_ref;

    if (argumentCount != 1)
    {
	gchar *mes =
	    g_strdup_printf("GType constructor expected 1 "
			    "argument, got %d \n",
			    argumentCount);
	seed_make_exception(exception, "ArgumentError", mes);
	g_free(mes);
	return (JSObjectRef) JSValueMakeNull(eng->context);
    }
    if (!JSValueIsObject(eng->context, arguments[0]))
    {
	seed_make_exception(exception, "ArgumentError",
			    "GType constructor expected a"
			    "class definition object. Got a nonobject");

	return (JSObjectRef)JSValueMakeNull(eng->context);
    }
    parent_ref = seed_object_get_property((JSObjectRef)arguments[0], 
					  "parent");
    class_init = seed_object_get_property((JSObjectRef)arguments[0], 
					  "class_init");
    instance_init = seed_object_get_property((JSObjectRef)arguments[0], 
					     "instance_init");
    name = seed_object_get_property((JSObjectRef)arguments[0], 
				    "name");

    new_name = seed_value_to_string(name, exception);
    if (!JSValueIsNumber(eng->context,
			 parent_ref))
    {
	seed_make_exception(exception, "TypeError",
			    "GType constructor expected"
			    " Gype for parent");
	
	return (JSObjectRef)JSValueMakeNull(eng->context);
    }
    if (!JSValueIsNull(eng->context, class_init) &&
	JSValueIsObject(eng->context, class_init) &&
	JSObjectIsFunction(eng->context, (JSObjectRef) class_init))
    {
	init_closure = seed_make_class_init_closure((JSObjectRef) class_init);
    }
    if (!JSValueIsNull(eng->context, instance_init) &&
	JSValueIsObject(eng->context, instance_init) &&
	JSObjectIsFunction(eng->context, (JSObjectRef) instance_init))
    {
	instance_init_closure =
	    seed_make_instance_init_closure((JSObjectRef) instance_init);
    }

    parent_type = (GType) seed_value_to_int(parent_ref, exception);

    g_type_query(parent_type, &query);
    type_info.class_size = query.class_size;
    type_info.instance_size = query.instance_size;
    type_info.class_init = (GClassInitFunc) init_closure;
    type_info.instance_init = (GInstanceInitFunc) instance_init_closure;

    constructor_ref = JSObjectMake(eng->context, gobject_constructor_class,
				   (gpointer) new_type);
    JSValueProtect(eng->context, constructor_ref);

    type_info.class_data = constructor_ref;

    new_type = g_type_register_static(parent_type, new_name, &type_info, 0);
    seed_gobject_get_class_for_gtype(new_type);
    JSObjectSetPrivate(constructor_ref, (gpointer) new_type);

    g_free(new_name);
    return JSObjectMake(eng->context, gobject_constructor_class,
			(gpointer) new_type);
}

void seed_gtype_init(void)
{
    JSClassDefinition gtype_def = kJSClassDefinitionEmpty;
    JSObjectRef gtype_constructor;

    gtype_def.callAsConstructor = seed_gtype_constructor_invoked;
    seed_gtype_class = JSClassCreate(&gtype_def);
    JSClassRetain(seed_gtype_class);

    gtype_constructor = JSObjectMake(eng->context, seed_gtype_class, 0);

    seed_object_set_property(eng->global, "GType", gtype_constructor);
}
