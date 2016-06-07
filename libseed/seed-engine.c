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
#include "seed-path.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include "seed-signals.h"

#include "config.h"

JSObjectRef function_proto;

JSObjectRef seed_obj_ref;
JSObjectRef ARGV_obj_ref;
JSObjectRef window_obj_ref;

GQuark qname;
GQuark qprototype;

JSClassRef gobject_signal_class;
JSClassRef gobject_named_constructor_class;
JSClassRef seed_struct_constructor_class;

JSContextGroupRef context_group;

JSStringRef defaults_script;

gchar* glib_message = 0;

GIBaseInfo* base_info_info = 0;

GQuark js_ref_quark;

guint seed_debug_flags = 0;              /* global seed debug flag */
gboolean seed_arg_print_version = FALSE; // Flag to print version and quit

pthread_key_t seed_next_gobject_wrapper_key;

#ifdef SEED_ENABLE_DEBUG
static const GDebugKey seed_debug_keys[]
  = { { "misc", SEED_DEBUG_MISC },
      { "finalization", SEED_DEBUG_FINALIZATION },
      { "initialization", SEED_DEBUG_INITIALIZATION },
      { "signal", SEED_DEBUG_SIGNAL },
      { "invocation", SEED_DEBUG_INVOCATION },
      { "structs", SEED_DEBUG_STRUCTS },
      { "construction", SEED_DEBUG_CONSTRUCTION },
      { "gtype", SEED_DEBUG_GTYPE },
      { "importer", SEED_DEBUG_IMPORTER },
      { "module", SEED_DEBUG_MODULE } };
#endif /* SEED_ENABLE_DEBUG */

static bool
seed_gobject_has_instance(JSContextRef ctx,
                          JSObjectRef constructor,
                          JSValueRef possible_instance,
                          JSValueRef* exception)
{
    GType constructor_type, value_type;
    if (JSValueIsNull(ctx, possible_instance)
        || !JSValueIsObject(ctx, possible_instance)
        || !JSValueIsObjectOfClass(ctx, possible_instance, gobject_class))
        return FALSE;

    constructor_type = (GType) JSObjectGetPrivate(constructor);
    value_type = G_OBJECT_TYPE(
      (GObject*) JSObjectGetPrivate((JSObjectRef) possible_instance));

    return g_type_is_a(value_type, constructor_type);
}

/**
 * seed_prepare_global_context:
 * @ctx: A #SeedContext on which to add the default set of global objects.
 *
 * Adds the default set of global objects (imports, GType, Seed, and print)
 * to a fresh #SeedContext.
 *
 */
void
seed_prepare_global_context(JSContextRef ctx)
{
    JSObjectRef global = JSContextGetGlobalObject(ctx);
    JSStringRef check_sprintf = JSStringCreateWithUTF8CString("sprintf");

    seed_object_set_property(ctx, global, "imports", importer);
    seed_object_set_property(ctx, global, "GType", seed_gtype_constructor);
    seed_object_set_property(ctx, global, "Seed", seed_obj_ref);
    seed_object_set_property(ctx, global, "ARGV", ARGV_obj_ref);
    seed_object_set_property(ctx, global, "window", window_obj_ref);
    seed_object_set_property(ctx, global, "print", seed_print_ref);
    seed_object_set_property(ctx, global, "printerr", seed_printerr_ref);

    /* No need to re-import Seed.js if not needed. */
    if (!JSObjectHasProperty(ctx, seed_obj_ref, check_sprintf)) {
        JSEvaluateScript(ctx, defaults_script, NULL, NULL, 0, NULL);
    }
    JSStringRelease(check_sprintf);
}

static JSObjectRef
seed_struct_constructor_invoked(JSContextRef ctx,
                                JSObjectRef constructor,
                                size_t argumentCount,
                                const JSValueRef arguments[],
                                JSValueRef* exception)
{
    GIBaseInfo* info = JSObjectGetPrivate(constructor);
    JSValueRef ret;
    JSObjectRef parameters = 0;

    if (argumentCount == 1) {
        if (!JSValueIsObject(ctx, arguments[0])) {

            // new GObject.GValue()  can accept anything as a argument...
            GType gtype
              = g_registered_type_info_get_g_type((GIRegisteredTypeInfo*) info);
            if (!g_type_is_a(gtype, G_TYPE_VALUE)) {
                seed_make_exception(ctx, exception, "ArgumentError",
                                    "Constructor expects object as argument");
                return (JSObjectRef) JSValueMakeNull(ctx);
            }
        }
        parameters = (JSObjectRef) arguments[0];
    }
    ret = seed_construct_struct_type_with_parameters(ctx, info, parameters,
                                                     exception);

    return (JSObjectRef) ret;
}

static JSObjectRef
seed_gobject_constructor_invoked(JSContextRef ctx,
                                 JSObjectRef constructor,
                                 size_t argumentCount,
                                 const JSValueRef arguments[],
                                 JSValueRef* exception)
{
    GType type;
    GParameter* params;
    GObjectClass* oclass;
    GObject* gobject;
    GParamSpec* param_spec;
    gchar* prop_name;
    gint i, nparams = 0, length, ri;
    JSObjectRef ret;
    JSPropertyNameArrayRef jsprops = 0;
    JSStringRef jsprop_name;
    JSValueRef jsprop_value;

    type = (GType) JSObjectGetPrivate(constructor);
    if (!type)
        return 0;

    oclass = g_type_class_ref(type);

    // Check for an exception in class init (which may have just been called
    // by g_type_class_ref, if this is the first construction of this class).
    // Bubble up the exception, and clear it from the class's qdata so that
    // this doesn't happen on subsequent construction.
    GQuark class_init_exception_q
      = g_quark_from_static_string("type-class-init-exception");
    JSValueRef class_init_exception
      = (JSValueRef) g_type_get_qdata(type, class_init_exception_q);
    if (class_init_exception) {
        *exception = class_init_exception;
        g_type_set_qdata(type, class_init_exception_q, NULL);
        return (JSObjectRef) JSValueMakeNull(ctx);
    }

    if (argumentCount > 1) {
        seed_make_exception(ctx, exception, "ArgumentError",
                            "Constructor expects"
                            " 1 argument, got %zd",
                            argumentCount);

        return (JSObjectRef) JSValueMakeNull(ctx);
    }

    if (argumentCount == 1) {
        if (!JSValueIsObject(ctx, arguments[0])) {
            seed_make_exception(ctx, exception, "ArgumentError",
                                "Constructor expects object as argument");
            g_type_class_unref(oclass);
            return (JSObjectRef) JSValueMakeNull(ctx);
        }

        jsprops = JSObjectCopyPropertyNames(ctx, (JSObjectRef) arguments[0]);
        nparams = JSPropertyNameArrayGetCount(jsprops);
    }
    ri = i = 0;

    params = g_new0(GParameter, nparams + 1);
    SEED_NOTE(INITIALIZATION, "Constructing object of type %s",
              g_type_name(type));

    pthread_setspecific(seed_next_gobject_wrapper_key,
                        seed_make_wrapper_for_type(ctx, type));

    while (i < nparams) {
        GType type;
        jsprop_name = JSPropertyNameArrayGetNameAtIndex(jsprops, i);

        length = JSStringGetMaximumUTF8CStringSize(jsprop_name);
        prop_name = g_alloca(length * sizeof(gchar));
        JSStringGetUTF8CString(jsprop_name, prop_name, length);

        param_spec = g_object_class_find_property(oclass, prop_name);

        jsprop_value = JSObjectGetProperty(ctx, (JSObjectRef) arguments[0],
                                           jsprop_name, NULL);

        if (param_spec == NULL) {
            JSObjectSetProperty(ctx, pthread_getspecific(
                                       seed_next_gobject_wrapper_key),
                                jsprop_name, jsprop_value, 0, NULL);
            ++i;
            continue;
        }
        // TODO: exception handling

        if (g_type_is_a(param_spec->value_type, G_TYPE_ENUM))
            type = G_TYPE_INT;
        else
            type = param_spec->value_type;

        seed_value_to_gvalue(ctx, jsprop_value, type, &params[ri].value,
                             exception);

        if (*exception) {
            g_free(params);
            JSPropertyNameArrayRelease(jsprops);
            pthread_setspecific(seed_next_gobject_wrapper_key, NULL);
            return 0;
        }
        params[ri].name = prop_name;

        ++i;
        ++ri;
    }

    if (jsprops)
        JSPropertyNameArrayRelease(jsprops);

    SEED_NOTE(INITIALIZATION,
              "G_TYPE_IS_INSTANTIATABLE  = %d  G_TYPE_IS_ABSTRACT = %d",
              G_TYPE_IS_INSTANTIATABLE(type), G_TYPE_IS_ABSTRACT(type));

    if (!G_TYPE_IS_INSTANTIATABLE(type) || G_TYPE_IS_ABSTRACT(type)) {
        seed_make_exception(ctx, exception, "ArgumentError",
                            "Type can not be created - not INSTANTIATABLE");

        return (JSObjectRef) JSValueMakeNull(ctx);
    }

    gobject = g_object_newv(type, ri, params);

    if (G_IS_INITIALLY_UNOWNED(gobject) && !g_object_is_floating(gobject))
        g_object_ref(gobject);
    else if (g_object_is_floating(gobject))
        g_object_ref_sink(gobject);

    if (!gobject)
        ret = (JSObjectRef) JSValueMakeNull(ctx);
    else
        ret = (JSObjectRef) seed_value_from_object(ctx, gobject, exception);

    for (i = 0; i < ri; i++) {
        g_value_unset(&params[i].value);
    }

    g_object_unref(gobject);

    g_type_class_unref(oclass);

    g_free(params);

    return ret;
}

static JSValueRef
seed_gobject_property_type(JSContextRef ctx,
                           JSObjectRef function,
                           JSObjectRef this_object,
                           size_t argumentCount,
                           const JSValueRef arguments[],
                           JSValueRef* exception)
{
    GParamSpec* spec;
    gchar* name;
    GObject* this;

    if (argumentCount != 1) {
        seed_make_exception(ctx, exception, "ArgumentError",
                            "__property_type expects 1 argument"
                            "got %zd",
                            argumentCount);
        return JSValueMakeNull(ctx);
    }

    this = seed_value_to_object(ctx, this_object, exception);
    name = seed_value_to_string(ctx, arguments[0], exception);

    spec = g_object_class_find_property(G_OBJECT_GET_CLASS(this), name);
    g_free(name);

    return seed_value_from_long(ctx, spec->value_type, exception);
}

static JSValueRef
seed_gobject_ref_count(JSContextRef ctx,
                       JSObjectRef function,
                       JSObjectRef this_object,
                       size_t argumentCount,
                       const JSValueRef arguments[],
                       JSValueRef* exception)
{
    GObject* this;

    this = seed_value_to_object(ctx, (JSValueRef) this_object, exception);

    return seed_value_from_int(ctx, this->ref_count, exception);
}

static void
seed_gobject_method_finalize(JSObjectRef method)
{
    GIBaseInfo* info = (GIBaseInfo*) JSObjectGetPrivate(method);
    if (info)
        g_base_info_unref(info);
}

typedef void (*InitMethodCallback)(gint* argc, gchar*** argv);

static gboolean
seed_gobject_init_build_argv(JSContextRef ctx,
                             JSObjectRef array,
                             SeedArgvPrivates* priv,
                             JSValueRef* exception)
{
    guint i, length;
    JSValueRef jsl;

    jsl = seed_object_get_property(ctx, array, "length");
    if (JSValueIsNull(ctx, jsl) || JSValueIsUndefined(ctx, jsl))
        return FALSE;

    length = seed_value_to_uint(ctx, jsl, exception);
    priv->argv = g_new(gchar*, length);
    priv->argc = length;

    for (i = 0; i < length; i++) {
        priv->argv[i] = seed_value_to_string(
          ctx, JSObjectGetPropertyAtIndex(ctx, array, i, exception), exception);
    }
    return TRUE;
}

static JSValueRef
seed_gobject_init_method_invoked(JSContextRef ctx,
                                 JSObjectRef function,
                                 JSObjectRef this_object,
                                 size_t argumentCount,
                                 const JSValueRef arguments[],
                                 JSValueRef* exception)
{
    GIBaseInfo* info;
    GTypelib* typelib;
    InitMethodCallback c;
    SeedArgvPrivates* priv = NULL;
    gboolean allocated = FALSE;

    if (argumentCount != 1 && argumentCount != 2) {
        seed_make_exception(ctx, exception, "ArgumentError",
                            "init method expects 1 argument, got %zd",
                            argumentCount);
        return JSValueMakeUndefined(ctx);
    }

    if (argumentCount == 1) {
        if (!JSValueIsNull(ctx, arguments[0])
            && !JSValueIsObject(ctx, arguments[0]))

        {
            seed_make_exception(
              ctx, exception, "ArgumentError",
              "init method expects an array object as argument");
            return JSValueMakeUndefined(ctx);
        }
        if (JSValueIsNull(ctx, arguments[0])) {
            priv = NULL;
        } else if (JSValueIsObjectOfClass(ctx, arguments[0], seed_argv_class)) {
            priv = JSObjectGetPrivate((JSObjectRef) arguments[0]);
        } else {
            priv = g_newa(SeedArgvPrivates, 1);
            if (!seed_gobject_init_build_argv(ctx, (JSObjectRef) arguments[0],
                                              priv, exception)) {
                seed_make_exception(ctx, exception, "ArgumentError",
                                    "Init method expects an array as argument");
                return JSValueMakeUndefined(ctx);
            }
            allocated = TRUE;
        }
    }

    info = JSObjectGetPrivate(function);
    typelib = g_base_info_get_typelib(info);
    g_typelib_symbol(typelib,
                     g_function_info_get_symbol((GIFunctionInfo*) info),
                     (gpointer*) &c);
    // Backwards compatibility
    if (!priv) {
        c(NULL, NULL);
        return JSValueMakeUndefined(ctx);
    }

    c(&priv->argc, &priv->argv);

    if (allocated)
        g_free(priv->argv);

    return JSValueMakeUndefined(ctx);
}

// Pre-process the arguments list to find which arguments needs to be skipped.
gint*
_process_skipped_arguments(const JSValueRef arguments[],
                           GIBaseInfo* info,
                           gint* out_skipped_args)
{
    int n_args = g_callable_info_get_n_args((GICallableInfo*) info);
    int skipped_args = 0;
    ;
    int i;

    gint* skip = g_new0(gint, n_args + 1);

    GITypeInfo* type_info = NULL;
    GIArgInfo* arg_info = NULL;

    for (i = 0; (i < (n_args)); i++) {
        arg_info = g_callable_info_get_arg((GICallableInfo*) info, i);
        type_info = g_arg_info_get_type(arg_info);
        GIDirection dir = g_arg_info_get_direction(arg_info);

        GITypeTag type_tag = g_type_info_get_tag(type_info);
        gint array_len_pos = g_type_info_get_array_length(type_info);

        if (dir == GI_DIRECTION_IN)
            if (type_tag == GI_TYPE_TAG_ARRAY
                && g_type_info_get_array_type(type_info) == GI_ARRAY_TYPE_C
                && array_len_pos >= 0) {
                skip[array_len_pos] = 1;
                skipped_args++;
            }

        g_base_info_unref((GIBaseInfo*) type_info);
        g_base_info_unref((GIBaseInfo*) arg_info);
        type_info = NULL;
        arg_info = NULL;
    }

    *out_skipped_args = skipped_args;
    return skip;
}

void
_add_jsvalue_in_array(JSContextRef ctx,
                      JSObjectRef dest,
                      gint pos,
                      JSValueRef obj)
{
    gchar* int_str = g_strdup_printf("%d", pos);
    seed_object_set_property(ctx, (JSObjectRef) dest, int_str, obj);
    g_free(int_str);
}

static JSValueRef
seed_gobject_method_invoked(JSContextRef ctx,
                            JSObjectRef function,
                            JSObjectRef this_object,
                            size_t argumentCount,
                            const JSValueRef arguments[],
                            JSValueRef* exception)
{
    GIBaseInfo* info;
    GObject* object = NULL;
    gboolean instance_method = TRUE;
    gboolean is_caller_allocates = FALSE;
    gboolean* caller_allocated;
    GIBaseInfo* iface_info = NULL;
    GArgument retval;
    GArgument* in_args;
    GArgument* out_args;
    GArgument* out_values;
    gint* skipped_args = NULL;
    gint* out_pos;
    gint first_out = -1;
    guint use_return_as_out = 0;
    guint n_args, n_in_args, n_out_args, i, j;
    guint in_args_pos, out_args_pos;
    GIArgInfo* arg_info = NULL;
    GITypeInfo* type_info = NULL;
    GITypeTag tag;
    GIDirection dir;
    GITransfer transfer;
    JSValueRef retval_ref;
    GError* error = 0;
    gint length_arg_pos = 0;
    guint64 array_len = 0;
    gint array_return_value_count = 0;
    gboolean return_only_one_out = false;

    info = JSObjectGetPrivate(function);

    // We just want to check if there IS an object, not actually throw an
    // exception if we don't get it.
    if (!this_object
        || !((object = seed_value_to_object(ctx, this_object, 0))
             || (object = seed_pointer_get_pointer(ctx, this_object))))
        instance_method = FALSE;

    n_args = g_callable_info_get_n_args((GICallableInfo*) info);

    out_pos = g_new0(gint, n_args + 1);
    in_args = g_new0(GArgument, n_args + 1);
    out_args = g_new0(GArgument, n_args + 1);
    out_values = g_new0(GArgument, n_args + 1);
    n_in_args = n_out_args = 0;
    caller_allocated = g_new0(gboolean, n_args + 1);

    // start by pushing the object onto the call stack.
    if (instance_method)
        in_args[n_in_args++].v_pointer = object;

    int out_skipped_args = 0;
    skipped_args
      = _process_skipped_arguments(arguments, info, &out_skipped_args);

    SEED_NOTE(INVOCATION, "Calling seed_object_method_invoked for function %s",
              g_base_info_get_name(info));

    // now loop through all the other args.
    for (i = 0, j = 0; (i < (n_args)); i++) {
        if (skipped_args[i]) {
            SEED_NOTE(INVOCATION, "Skipping parameter %d", i);
            n_in_args++;
            j++;
            continue;
        }

        out_pos[i] = -1;
        arg_info = g_callable_info_get_arg((GICallableInfo*) info, i);
        dir = g_arg_info_get_direction(arg_info);
        type_info = g_arg_info_get_type(arg_info);
        transfer = g_arg_info_get_ownership_transfer(arg_info);
        is_caller_allocates = FALSE;

#if GOBJECT_INTROSPECTION_VERSION > 0x000613
        is_caller_allocates = (dir == GI_DIRECTION_OUT)
                              && g_arg_info_is_caller_allocates(arg_info);

        /* caller allocates only applies to structures but GI has
        * no way to denote that yet, so we only use caller allocates
        * if we see  a structure
        */
        if (is_caller_allocates) {
            GITypeTag type_tag = g_type_info_get_tag(type_info);

            is_caller_allocates = FALSE;

            if (type_tag == GI_TYPE_TAG_INTERFACE) {
                GIInfoType info_type;

                iface_info = g_type_info_get_interface(type_info);
                g_assert(info != NULL);
                info_type = g_base_info_get_type(iface_info);

                if (info_type == GI_INFO_TYPE_STRUCT)
                    is_caller_allocates = TRUE;
            }
        }
#endif

        SEED_NOTE(INVOCATION,
                  "Converting arg: %s (%d) of function %s, exception is %p",
                  g_base_info_get_name((GIBaseInfo*) arg_info), i,
                  g_base_info_get_name(info), exception);

        if (i + 1 > argumentCount + out_skipped_args) {

            if (dir == GI_DIRECTION_OUT) {
                GArgument* out_value = &out_values[n_out_args];
                out_values[n_out_args].v_pointer = NULL;
                out_args[n_out_args].v_pointer = out_value;

                out_pos[i] = n_out_args;

                if (is_caller_allocates) {
                    gsize size
                      = g_struct_info_get_size((GIStructInfo*) iface_info);
                    out_args[n_out_args].v_pointer = g_malloc0(size);
                    out_values[n_out_args].v_pointer
                      = out_args[n_out_args].v_pointer;
                    caller_allocated[i] = TRUE;
                }
                n_out_args++;
            } else
                in_args[n_in_args++].v_pointer = 0;

        } else if (dir == GI_DIRECTION_IN || dir == GI_DIRECTION_INOUT) {

            if (!g_arg_info_may_be_null(arg_info)) {
                gboolean is_null
                  = (!arguments[i - j] || JSValueIsNull(ctx, arguments[i - j]));

                if (!is_null && (g_type_info_get_tag(type_info)
                                 == GI_TYPE_TAG_INTERFACE)) {
                    /* see if the pointer is null for struct/unions. */
                    GIBaseInfo* interface = g_type_info_get_interface(
                      type_info);
                    GIInfoType interface_type = g_base_info_get_type(interface);

                    gboolean arg_is_object
                      = JSValueIsObject(ctx, arguments[i - j]);

                    gboolean is_gvalue
                      = (interface_type == GI_INFO_TYPE_STRUCT)
                        && !g_strcmp0(g_base_info_get_name(
                                        (GIBaseInfo*) interface),
                                      "Value")
                        && !g_strcmp0(g_base_info_get_namespace(
                                        (GIBaseInfo*) interface),
                                      "GObject");

                    gboolean is_struct_or_union
                      = (interface_type == GI_INFO_TYPE_STRUCT
                         || interface_type == GI_INFO_TYPE_UNION);

                    /* this test ignores non-objects being sent where interfaces
                       are expected
                       hopefully our type manipluation code will pick that up.
                       The only exception to this is GObject.GValues which are
                       arrays.
                       */
                    if (!is_gvalue && is_struct_or_union && arg_is_object
                        && (seed_pointer_get_pointer(ctx, arguments[i - j])
                            == 0)) {
                        is_null = TRUE;
                    }
                    g_base_info_unref(interface);
                }

                if (is_null) {
                    GIBaseInfo* ctr
                      = g_base_info_get_container((GIBaseInfo*) info);
                    // note - ctr does not need unref'ing (see ginfo.c source
                    // for why)

                    // RE-INSTATE THIS CODE LATER.. - when gtk etc. has be
                    // release with fixes
                    // seed_make_exception (ctx, exception,
                    //   "ArgumentError",
                    g_warning("ArgumentError - probably due to incorrect gir "
                              "file (which may be fixed upstream)"
                              " argument %d must not be null for"
                              " function: %s%s%s \n",
                              i + 1,
                              ctr ? g_base_info_get_name((GIBaseInfo*) ctr)
                                  : "",
                              ctr ? "." : "",
                              g_base_info_get_name((GIBaseInfo*) info));
                    // goto arg_error;
                }
            }

            guint out_length = 0;
            if (!seed_value_to_gi_argument_with_out_length(
                  ctx, arguments[i - j], type_info, transfer,
                  &in_args[n_in_args++], &out_length, exception)) {
                seed_make_exception(ctx, exception, "ArgumentError",
                                    "Unable to make argument %d for"
                                    " function: %s. \n",
                                    i + 1,
                                    g_base_info_get_name((GIBaseInfo*) info));

                retval_ref = JSValueMakeNull(ctx);
                goto invoke_return;
            }

            // check if what we did we did on an array,
            // if yes, fill the g_type_info_get_array_length() position with the
            // size of the array and mark it as already skipped.
            GITypeTag type_tag = g_type_info_get_tag(type_info);
            gint array_len_pos = g_type_info_get_array_length(type_info);
            if (type_tag == GI_TYPE_TAG_ARRAY
                && g_type_info_get_array_type(type_info) == GI_ARRAY_TYPE_C
                && array_len_pos >= 0) {
                skipped_args[array_len_pos] = TRUE;
                in_args[array_len_pos + 1].v_uint64 = out_length;
            }

            if (dir == GI_DIRECTION_INOUT) {
                GArgument* out_value = &out_values[n_out_args];
                out_args[n_out_args++].v_pointer = out_value;
                out_pos[i] = n_out_args;
            }

        } else if (dir == GI_DIRECTION_OUT) {
            GArgument* out_value = &out_values[n_out_args];
            out_values[n_out_args].v_pointer = NULL;
            out_args[n_out_args].v_pointer = out_value;
            out_pos[i] = n_out_args;
            if (is_caller_allocates) {
                gsize size = g_struct_info_get_size((GIStructInfo*) iface_info);
                out_args[n_out_args].v_pointer = g_malloc0(size);
                out_values[n_out_args].v_pointer
                  = out_args[n_out_args].v_pointer;
                caller_allocated[i] = TRUE;
            }
            n_out_args++;
            first_out = first_out > -1 ? first_out : i;
        }

        g_base_info_unref((GIBaseInfo*) type_info);
        g_base_info_unref((GIBaseInfo*) arg_info);
        if (iface_info)
            g_base_info_unref(iface_info);
        iface_info = NULL;
        type_info = NULL;
        arg_info = NULL;
    }

    // --- Finished building the args, now call the method / function.

    SEED_NOTE(INVOCATION, "Invoking method: %s with %d 'in' arguments"
                          " and %d 'out' arguments",
              g_base_info_get_name(info), n_in_args, n_out_args);
    if (!g_function_info_invoke((GIFunctionInfo*) info, in_args, n_in_args,
                                out_args, n_out_args, &retval, &error)) {

        // failed...
        seed_make_exception_from_gerror(ctx, exception, error);
        g_error_free(error);
        retval_ref = JSValueMakeNull(ctx);
        goto invoke_return;
    }

    // -- returned OK..

    type_info = g_callable_info_get_return_type((GICallableInfo*) info);
    tag = g_type_info_get_tag(type_info);

    // might need to add g_type_info_is_pointer (type_info) check here..

    // We force a return array in the following situation:
    // There's more than one n_out_args OR
    // There's one out_arg AND tag != GI_TYPE_TAG_VOID
    // AND, of course, if it's not an INTERFACE.
    gboolean force_return_array = false;
    force_return_array = (tag != GI_TYPE_TAG_INTERFACE);

    if (force_return_array) {
        if (n_out_args + !!(tag != GI_TYPE_TAG_VOID) > 1) {
            retval_ref = JSObjectMake(ctx, NULL, NULL);
            use_return_as_out = 1;
        }
    }

    if (tag == GI_TYPE_TAG_VOID) {
        // * if we have no out args - returns undefined
        // * if we have *one* out_arg but no return type,
        //   do nothing here and return the OUT as return later.
        // * otherwise we return an object, and put the return values into that
        //   along with supporting the old object.value way
        if (n_out_args < 1)
            retval_ref = JSValueMakeUndefined(ctx);
        else if (n_out_args == 1){
            return_only_one_out = true;
            use_return_as_out = 1;
        }
        else {
            retval_ref = JSObjectMake(ctx, NULL, NULL);
            use_return_as_out = 1;
        }
    } else {

        // not a void return.

        GIBaseInfo* interface;
        gboolean sunk = FALSE;

        // for most returns we just pump it though our type convert calls.
        // however gobjects are different, if they are not owned by anybody,
        // then we add a ref so that it can not be destroyed during this
        // process.
        // this ref is removed after we have converted it to a JSObject.

        if (tag == GI_TYPE_TAG_INTERFACE) {
            GIInfoType interface_type;

            interface = g_type_info_get_interface(type_info);
            interface_type = g_base_info_get_type(interface);
            g_base_info_unref(interface);

            if (interface_type == GI_INFO_TYPE_OBJECT
                || interface_type == GI_INFO_TYPE_INTERFACE) {
                if (G_IS_OBJECT(retval.v_pointer)) {
                    sunk = G_IS_INITIALLY_UNOWNED(G_OBJECT(retval.v_pointer));
                    if (sunk)
                        g_object_ref_sink(G_OBJECT(retval.v_pointer));
                }
            }
        }

        length_arg_pos = g_type_info_get_array_length(type_info);
        SEED_NOTE(INVOCATION, "length_arg_pos=%d\n", length_arg_pos);
        if (length_arg_pos < 0) {
            array_len = 0;
        } else {
            array_len = (&out_values[out_pos[length_arg_pos]])->v_uint32;
        }
        SEED_NOTE(INVOCATION, "array_len=%" G_GUINT64_FORMAT "\n", array_len);

        JSValueRef jsout_val
          = seed_value_from_gi_argument_full(ctx, &retval, type_info, exception,
                                             array_len, tag);

        if (use_return_as_out && force_return_array) {
            _add_jsvalue_in_array(ctx, (JSObjectRef) retval_ref,
                                  array_return_value_count++, jsout_val);
        } else
            retval_ref = jsout_val;

        if (sunk)
            g_object_unref(G_OBJECT(retval.v_pointer));
        else
            seed_gi_release_arg(g_callable_info_get_caller_owns(
                                  (GICallableInfo*) info),
                                type_info, &retval);
    }
    if (type_info)
        g_base_info_unref((GIBaseInfo*) type_info);
    type_info = NULL;

    // finished with return.. now go thorugh the args and handle any out/inout
    // etc..

    in_args_pos = out_args_pos = 0;
    for (i = 0; (i < n_args); i++) {
        JSValueRef jsout_val;
        arg_info = g_callable_info_get_arg((GICallableInfo*) info, i);
        dir = g_arg_info_get_direction(arg_info);
        type_info = g_arg_info_get_type(arg_info);
        // since we succesfully called, we can presume that

        if (dir == GI_DIRECTION_IN || dir == GI_DIRECTION_INOUT) {
            seed_gi_release_in_arg(
              g_arg_info_get_ownership_transfer(arg_info), type_info,
              &in_args[in_args_pos + (instance_method ? 1 : 0)]);
            in_args_pos++;

            g_base_info_unref((GIBaseInfo*) type_info);
            g_base_info_unref((GIBaseInfo*) arg_info);
            type_info = NULL;
            arg_info = NULL;

            continue;
        }

        // we are now only dealing with OUT arguments.

        // if the type_info is an array with a length position, we
        // need to send that as well, so it can be used to build the seed value.
        {
            length_arg_pos = g_type_info_get_array_length(type_info);
            array_len = 0;
            if (length_arg_pos > -1) {
                GIArgInfo* array_arg_info;
                GITypeInfo* array_type_info;
                GArgument* array_len_arg;

                g_assert(out_pos[length_arg_pos] > -1);

                array_arg_info = g_callable_info_get_arg((GICallableInfo*) info,
                                                         length_arg_pos);
                array_type_info = g_arg_info_get_type(array_arg_info);

                array_len_arg = &out_values[out_pos[length_arg_pos]];

                array_len = 0;
                switch (g_type_info_get_tag(array_type_info)) {
                    // not sure if the non-unsigned  versions are need as length
                    // should not be -ve..
                    case GI_TYPE_TAG_INT8:
                        array_len = (guint64) array_len_arg->v_int8;
                        break;
                    case GI_TYPE_TAG_UINT8:
                        array_len = (guint64) array_len_arg->v_uint8;
                        break;
                    case GI_TYPE_TAG_INT16:
                        array_len = (guint64) array_len_arg->v_int16;
                        break;
                    case GI_TYPE_TAG_UINT16:
                        array_len = (guint64) array_len_arg->v_uint16;
                        break;
                    case GI_TYPE_TAG_INT32:
                        array_len = (guint64) array_len_arg->v_int32;
                        break;
                    case GI_TYPE_TAG_UINT32:
                        array_len = (guint64) array_len_arg->v_uint32;
                        break;
                    case GI_TYPE_TAG_INT64:
                        array_len = (guint64) array_len_arg->v_int64;
                        break;
                    case GI_TYPE_TAG_UINT64:
                        array_len = (guint64) array_len_arg->v_uint64;
                        break;
                    default:
                        g_assert(1 == 0);
                        break;
                }

                // this may work, but the above should be more accurate..
                // array_len =  (&out_values[  out_pos[length_arg_pos]
                // ])->v_uint32;

                SEED_NOTE(INVOCATION,
                          "Getting length from OUTPOS=%d,  ORIGPOS=%d :  "
                          "result = %" G_GUINT64_FORMAT,
                          out_pos[length_arg_pos], length_arg_pos, array_len);
                // free stuff.

                g_base_info_unref((GIBaseInfo*) array_type_info);
                g_base_info_unref((GIBaseInfo*) array_arg_info);
            }

            jsout_val = seed_value_from_gi_argument_full(
              ctx, &out_values[out_args_pos], type_info, exception, array_len,
              g_type_info_get_tag(type_info));
        }
        /* caller allocates only applies to structures but GI has
         * no way to denote that yet, so we only use caller allocates
         * if we see  a structure
         */

        if (caller_allocated[i]) {

            // this is the old python code..
            // if we are going to do this, the caching the iface_info at the top
            // would be a better idea.

            // if (g_registered_type_info_get_g_type ( (GIRegisteredTypeInfo *)
            // iface_info) == G_TYPE_VALUE)
            //     g_value_unset ( (GValue *) state->args[i]);

            // clear the caller allocated flag so it's not free'd at the end..
            caller_allocated[i] = FALSE;
        }

        // old ? depreciated ? way to handle out args -> set 'value' on object
        // that was send through.
        out_args_pos++;

        if ((i < argumentCount) && !JSValueIsNull(ctx, arguments[i])
            && JSValueIsObject(ctx, arguments[i])) {
            seed_object_set_property(ctx, (JSObjectRef) arguments[i], "value",
                                     jsout_val);
        }

        // if we add it to the return argument and/or the first out arguement

        if (use_return_as_out) {
            if (return_only_one_out) {
                retval_ref = jsout_val;
            } else if (force_return_array) {
                _add_jsvalue_in_array(ctx, (JSObjectRef) retval_ref,
                                      array_return_value_count++, jsout_val);
            } else {
                seed_object_set_property(ctx, (JSObjectRef) retval_ref,
                                         g_base_info_get_name(
                                           (GIBaseInfo*) arg_info),
                                         jsout_val);
            }
        }

        if ((first_out > -1) && !JSValueIsNull(ctx, arguments[first_out])
            && JSValueIsObject(ctx, arguments[first_out]))

        {
            seed_object_set_property(ctx, (JSObjectRef) arguments[first_out],
                                     g_base_info_get_name(
                                       (GIBaseInfo*) arg_info),
                                     jsout_val);
        }

        g_base_info_unref((GIBaseInfo*) arg_info);
        g_base_info_unref((GIBaseInfo*) type_info);
        type_info = NULL;
        arg_info = NULL;
    }

invoke_return:
    SEED_NOTE(INVOCATION, "END seed_object_method_invoked for function %s",
              g_base_info_get_name(info));

    // clean up everything..
    for (i = 0; (i < (n_args)); i++)
        if (caller_allocated[i])
            g_free(out_args[out_pos[i]].v_pointer);

    g_free(caller_allocated);

    if (type_info)
        g_base_info_unref((GIBaseInfo*) type_info);
    if (arg_info)
        g_base_info_unref((GIBaseInfo*) arg_info);
    if (iface_info)
        g_base_info_unref(iface_info);

    g_free(in_args);
    g_free(out_args);
    g_free(out_pos);
    g_free(out_values);
    return retval_ref;
}

static JSObjectRef
seed_gobject_named_constructor_invoked(JSContextRef ctx,
                                       JSObjectRef constructor,
                                       size_t argumentCount,
                                       const JSValueRef arguments[],
                                       JSValueRef* exception)
{
    return (JSObjectRef) seed_gobject_method_invoked(ctx, constructor, NULL,
                                                     argumentCount, arguments,
                                                     exception);
}

void
seed_gobject_define_property_from_function_info(JSContextRef ctx,
                                                GIFunctionInfo* info,
                                                JSObjectRef object,
                                                gboolean instance)
{
    GIFunctionInfoFlags flags;
    JSObjectRef method_ref;
    const gchar* name;

    // if (g_base_info_is_deprecated ((GIBaseInfo *) info))
    // g_printf("Not defining deprecated symbol: %s \n",
    // g_base_info_get_name((GIBaseInfo *)info));

    flags = g_function_info_get_flags(info);

    if (instance && (flags & GI_FUNCTION_IS_CONSTRUCTOR)) {
        return;
    }

    method_ref = JSObjectMake(ctx, gobject_method_class,
                              g_base_info_ref((GIBaseInfo*) info));

    JSObjectSetPrototype(ctx, method_ref, function_proto);

    name = g_base_info_get_name((GIBaseInfo*) info);
    if (!g_strcmp0(name, "new")) {
        // To be compatible with gjs, we need to have new as function, too.
        seed_object_set_property(ctx, object, name, method_ref);
        name = "c_new";
    }
    seed_object_set_property(ctx, object, name, method_ref);
    /*
      //  Disabled as this crashes in a recursive loop now
    seed_object_set_property (ctx, method_ref, "info",
                              seed_make_struct (ctx,
                                                g_base_info_ref ((GIBaseInfo *)
                                                                 info),
                                                base_info_info));
    */
}

static void
seed_gobject_add_methods_for_interfaces(JSContextRef ctx,
                                        GIObjectInfo* oinfo,
                                        JSObjectRef object)
{
    GIInterfaceInfo* interface;
    GIFunctionInfo* function;
    gint n_interfaces, i, n_functions, k;

    n_interfaces = g_object_info_get_n_interfaces(oinfo);

    for (i = 0; i < n_interfaces; i++) {
        interface = g_object_info_get_interface(oinfo, i);

        n_functions = g_interface_info_get_n_methods(interface);
        for (k = 0; k < n_functions; k++) {
            function = g_interface_info_get_method(interface, k);
            seed_gobject_define_property_from_function_info(ctx, function,
                                                            object, TRUE);
        }
        g_base_info_unref((GIBaseInfo*) interface);
    }
}

static void
seed_gobject_add_methods_for_type(JSContextRef ctx,
                                  GIObjectInfo* oinfo,
                                  JSObjectRef object)
{
    gint n_methods;
    gint i;
    GIFunctionInfo* info;

    n_methods = g_object_info_get_n_methods(oinfo);

    for (i = 0; i < n_methods; i++) {
        info = g_object_info_get_method(oinfo, i);
        seed_gobject_define_property_from_function_info(ctx, info, object,
                                                        TRUE);
        g_base_info_unref((GIBaseInfo*) info);
    }
}

JSClassRef
seed_gobject_get_class_for_gtype(JSContextRef ctx, GType type)
{
    JSClassDefinition def;
    GType parent;
    JSClassRef ref;
    JSClassRef parent_class = 0;
    GIBaseInfo* info;
    JSObjectRef prototype_obj;
    JSObjectRef parent_prototype;

    if ((ref = g_type_get_qdata(type, qname)) != NULL) {
        return ref;
    }

    info = g_irepository_find_by_gtype(g_irepository_get_default(), type);

    memset(&def, 0, sizeof(JSClassDefinition));

    def.className = g_type_name(type);
    if ((parent = g_type_parent(type)))
        parent_class = seed_gobject_get_class_for_gtype(ctx, parent);
    def.parentClass = parent_class;
    def.attributes = kJSClassAttributeNoAutomaticPrototype;

    prototype_obj = JSObjectMake(ctx, 0, 0);
    if (parent) {
        parent_prototype = seed_gobject_get_prototype_for_gtype(parent);
        if (parent_prototype)
            JSObjectSetPrototype(ctx, prototype_obj, parent_prototype);
    }

    ref = JSClassCreate(&def);
    JSClassRetain(ref);

    JSValueProtect(ctx, prototype_obj);

    g_type_set_qdata(type, qname, ref);
    g_type_set_qdata(type, qprototype, prototype_obj);

    if (info && (g_base_info_get_type(info) == GI_INFO_TYPE_OBJECT)) {
        seed_gobject_add_methods_for_interfaces(ctx, (GIObjectInfo*) info,
                                                prototype_obj);
        seed_gobject_add_methods_for_type(ctx, (GIObjectInfo*) info,
                                          prototype_obj);
        g_base_info_unref(info);
    } else {
        GType* interfaces;
        GIFunctionInfo* function;
        GIBaseInfo* interface;
        gint n_functions, k;
        guint i, n;

        interfaces = g_type_interfaces(type, &n);
        for (i = 0; i < n; i++) {
            interface = g_irepository_find_by_gtype(0, interfaces[i]);
            if (!interface)
                break;
            n_functions
              = g_interface_info_get_n_methods((GIInterfaceInfo*) interface);
            for (k = 0; k < n_functions; k++) {
                function
                  = g_interface_info_get_method((GIInterfaceInfo*) interface,
                                                k);
                seed_gobject_define_property_from_function_info(ctx, function,
                                                                prototype_obj,
                                                                TRUE);
            }
        }
    }

    return ref;
}

JSObjectRef
seed_gobject_get_prototype_for_gtype(GType type)
{
    JSObjectRef prototype = 0;
    while (type && !prototype) {
        prototype = g_type_get_qdata(type, qprototype);
        type = g_type_parent(type);
    }

    return prototype;
}

static void
seed_gobject_finalize(JSObjectRef object)
{
    GObject* gobject;
    JSObjectRef js_ref;

    gobject = (GObject*) JSObjectGetPrivate((JSObjectRef) object);
    if (!gobject) {
        SEED_NOTE(FINALIZATION,
                  "Attempting to finalize already destroyed object.");
        return;
    }

    SEED_NOTE(FINALIZATION, "%s at %p (%d refs)",
              g_type_name(G_OBJECT_TYPE(gobject)), gobject, gobject->ref_count);

    js_ref = g_object_get_qdata(gobject, js_ref_quark);
    if (js_ref) {
        /* Steal the qdata here as otherwise we will call
         * JSValueUnprotect() from the destroy notify of
         * the qdata, which is not allowed to be called
         * from a finalizer!
         */
        g_object_steal_qdata(gobject, js_ref_quark);

        g_object_remove_toggle_ref(gobject, seed_toggle_ref, js_ref);
    } else {
        g_object_run_dispose(gobject);
    }
}

static JSValueRef
seed_gobject_get_property(JSContextRef context,
                          JSObjectRef object,
                          JSStringRef property_name,
                          JSValueRef* exception)
{
    GParamSpec* spec;
    GObject* b;
    GValue gval = { 0 };
    char* cproperty_name;
    gint length;
    JSValueRef ret;
    guint i;
    gsize len;

    b = seed_value_to_object(context, (JSValueRef) object, exception);
    if (!b)
        return 0;

    length = JSStringGetMaximumUTF8CStringSize(property_name);
    cproperty_name = g_alloca(length * sizeof(gchar));
    JSStringGetUTF8CString(property_name, cproperty_name, length);

    spec = g_object_class_find_property(G_OBJECT_GET_CLASS(b), cproperty_name);

    if (!spec) {
        len = strlen(cproperty_name) - 1;
        for (i = 0; i < len; i++) {
            if (cproperty_name[i] == '_')
                cproperty_name[i] = '-';
        }
        spec
          = g_object_class_find_property(G_OBJECT_GET_CLASS(b), cproperty_name);
        if (spec)
            goto found;
        else {
            GIFieldInfo* field = NULL;
            GIBaseInfo* info
              = (GIBaseInfo*) g_irepository_find_by_gtype(0, G_OBJECT_TYPE(b));
            gint n;
            const gchar* name;

            for (i = 0; i < len; i++) {
                if (cproperty_name[i] == '-')
                    cproperty_name[i] = '_';
            }

            if (!info) {
                return NULL;
            }

            n = g_object_info_get_n_fields((GIObjectInfo*) info);
            for (i = 0; i < n; i++) {
                field = g_object_info_get_field((GIObjectInfo*) info, i);
                name = g_base_info_get_name((GIBaseInfo*) field);

                if (!g_strcmp0(name, cproperty_name))
                    goto found_field;
                else {
                    g_base_info_unref((GIBaseInfo*) field);
                    field = 0;
                }
            }
        found_field:
            if (field) {
                ret = seed_field_get_value(context, b, field, exception);
                g_base_info_unref((GIBaseInfo*) info);
                return ret;
            }
            g_base_info_unref((GIBaseInfo*) info);
        }
        return NULL;
    }
found:

    g_value_init(&gval, spec->value_type);
    g_object_get_property(b, cproperty_name, &gval);
    ret = seed_value_from_gvalue(context, &gval, exception);
    g_value_unset(&gval);

    return (JSValueRef) ret;
}

static bool
seed_gobject_set_property(JSContextRef context,
                          JSObjectRef object,
                          JSStringRef property_name,
                          JSValueRef value,
                          JSValueRef* exception)
{
    GParamSpec* spec = 0;
    GObject* obj;
    GValue gval = { 0 };
    GType type;
    gchar* cproperty_name;
    gsize length;
    gsize i, len;

    if (pthread_getspecific(seed_next_gobject_wrapper_key)
        || JSValueIsNull(context, value))
        return 0;

    obj = seed_value_to_object(context, object, 0);

    length = JSStringGetMaximumUTF8CStringSize(property_name);
    cproperty_name = g_alloca(length * sizeof(gchar));
    JSStringGetUTF8CString(property_name, cproperty_name, length);

    spec
      = g_object_class_find_property(G_OBJECT_GET_CLASS(obj), cproperty_name);

    if (!spec) {
        len = strlen(cproperty_name);
        for (i = 0; i < len; i++) {
            if (cproperty_name[i] == '_')
                cproperty_name[i] = '-';
        }
        spec = g_object_class_find_property(G_OBJECT_GET_CLASS(obj),
                                            cproperty_name);
        if (!spec) {
            return FALSE;
        }
    }

    if (g_type_is_a(spec->value_type, G_TYPE_ENUM))
        type = G_TYPE_LONG;
    else
        type = spec->value_type;

    seed_value_to_gvalue(context, value, type, &gval, exception);
    if (*exception) {
        return FALSE;
    }

    if (glib_message) {
        g_free(glib_message);
        glib_message = 0;
    }
    g_object_set_property(obj, cproperty_name, &gval);
    if (glib_message != 0) {
        seed_make_exception(context, exception, "PropertyError", glib_message,
                            NULL);

        return FALSE;
    }

    g_value_unset(&gval);

    return TRUE;
}

static JSValueRef
seed_gobject_convert_to_type(JSContextRef ctx,
                             JSObjectRef object,
                             JSType type,
                             JSValueRef* exception)
{
    GObject* obj;
    gchar* as_string;

    if (type == kJSTypeString) {
        JSValueRef ret;
        obj = (GObject*) JSObjectGetPrivate(object);

        as_string
          = g_strdup_printf("[gobject %s %p]", G_OBJECT_TYPE_NAME(obj), obj);
        ret = seed_value_from_string(ctx, as_string, exception);
        g_free(as_string);

        return ret;
    }
    return FALSE;
}

static JSValueRef
seed_gobject_constructor_convert_to_type(JSContextRef ctx,
                                         JSObjectRef object,
                                         JSType type,
                                         JSValueRef* exception)
{
    GType gtype;
    gchar* as_string;

    if (type == kJSTypeString) {
        JSValueRef ret;
        gtype = (GType) JSObjectGetPrivate(object);

        as_string
          = g_strdup_printf("[gobject_constructor %s]", g_type_name(gtype));
        ret = seed_value_from_string(ctx, as_string, exception);
        g_free(as_string);

        return ret;
    }
    return FALSE;
}

JSStaticFunction gobject_static_funcs[]
  = { { "__debug_ref_count", seed_gobject_ref_count, 0 },
      { "__property_type", seed_gobject_property_type, 0 },
      { "connect", seed_gobject_signal_connect_by_name, 0 },
      { 0, 0, 0 } };

JSClassDefinition gobject_def = {
    0,                                     /* Version, always 0 */
    kJSClassAttributeNoAutomaticPrototype, /* JSClassAttributes */
    "gobject",                             /* Class Name */
    NULL,                                  /* Parent Class */
    NULL,                                  /* Static Values */
    gobject_static_funcs,                  /* Static Functions */
    NULL,
    seed_gobject_finalize,       /* Finalize */
    NULL,                        /* Has Property */
    seed_gobject_get_property,   /* Get Property */
    seed_gobject_set_property,   /* Set Property */
    NULL,                        /* Delete Property */
    NULL,                        /* Get Property Names */
    NULL,                        /* Call As Function */
    NULL,                        /* Call As Constructor */
    NULL,                        /* Has Instance */
    seed_gobject_convert_to_type /* Convert To Type */
};

JSClassDefinition gobject_method_def = {
    0, /* Version, always 0 */
    0,
    "gobject_method", /* Class Name */
    NULL,             /* Parent Class */
    NULL,             /* Static Values */
    NULL,             /* Static Functions */
    NULL,
    seed_gobject_method_finalize, /* Finalize */
    NULL,                         /* Has Property */
    NULL,                         /* Get Property */
    NULL,                         /* Set Property */
    NULL,                         /* Delete Property */
    NULL,                         /* Get Property Names */
    seed_gobject_method_invoked,  /* Call As Function */
    NULL,                         /* Call As Constructor */
    NULL,                         /* Has Instance */
    NULL                          /* Convert To Type */
};

JSClassDefinition gobject_init_method_def = {
    0, /* Version, always 0 */
    0,
    "init_method", /* Class Name */
    NULL,          /* Parent Class */
    NULL,          /* Static Values */
    NULL,          /* Static Functions */
    NULL,
    seed_gobject_method_finalize,     /* Finalize */
    NULL,                             /* Has Property */
    NULL,                             /* Get Property */
    NULL,                             /* Set Property */
    NULL,                             /* Delete Property */
    NULL,                             /* Get Property Names */
    seed_gobject_init_method_invoked, /* Call As Function */
    NULL,                             /* Call As Constructor */
    NULL,                             /* Has Instance */
    NULL                              /* Convert To Type */
};

JSClassDefinition seed_callback_def = {
    0,                     /* Version, always 0 */
    0,    "seed_callback", /* Class Name */
    NULL,                  /* Parent Class */
    NULL,                  /* Static Values */
    NULL,                  /* Static Functions */
    NULL, NULL,            /* Finalize */
    NULL,                  /* Has Property */
    NULL,                  /* Get Property */
    NULL,                  /* Set Property */
    NULL,                  /* Delete Property */
    NULL,                  /* Get Property Names */
    NULL,                  /* Call As Function */
    NULL,                  /* Call As Constructor */
    NULL,                  /* Has Instance */
    NULL                   /* Convert To Type */
};

JSClassDefinition gobject_constructor_def
  = { 0, /* Version, always 0 */
      0,
      "gobject_constructor", /* Class Name */
      NULL,                  /* Parent Class */
      NULL,                  /* Static Values */
      NULL,                  /* Static Functions */
      NULL,
      NULL,                             /* Finalize */
      NULL,                             /* Has Property */
      NULL,                             /* Get Property */
      NULL,                             /* Set Property */
      NULL,                             /* Delete Property */
      NULL,                             /* Get Property Names */
      NULL,                             /* Call As Function */
      seed_gobject_constructor_invoked, /* Call As Constructor */
      seed_gobject_has_instance,        /* Has Instance */
      seed_gobject_constructor_convert_to_type };

JSClassDefinition gobject_named_constructor_def
  = { 0,                              /* Version, always 0 */
      0, "gobject_named_constructor", /* Class Name */
      NULL,                           /* Parent Class */
      NULL,                           /* Static Values */
      NULL,                           /* Static Functions */
      NULL, NULL,                     /* Finalize */
      NULL,                           /* Has Property */
      NULL,                           /* Get Property */
      NULL,                           /* Set Property */
      NULL,                           /* Delete Property */
      NULL,                           /* Get Property Names */
      // To be compatible with gjs you need to be able to call named
      // constructors without the new in front
      seed_gobject_method_invoked,            /* Call As Function */
      seed_gobject_named_constructor_invoked, /* Call As Constructor */
      seed_gobject_has_instance,              /* Has Instance */
      seed_gobject_constructor_convert_to_type };

JSClassDefinition struct_constructor_def = {
    0, /* Version, always 0 */
    0,
    "struct_constructor", /* Class Name */
    NULL,                 /* Parent Class */
    NULL,                 /* Static Values */
    NULL,                 /* Static Functions */
    NULL,
    NULL,                            /* Finalize */
    NULL,                            /* Has Property */
    NULL,                            /* Get Property */
    NULL,                            /* Set Property */
    NULL,                            /* Delete Property */
    NULL,                            /* Get Property Names */
    NULL,                            /* Call As Function */
    seed_struct_constructor_invoked, /* Call As Constructor */
    NULL,                            /* Has Instance */
    NULL                             /* Convert To Type */
};

/**
 * seed_create_function:
 * @ctx: A valid #SeedContext
 * @name: The name of the function (used in exceptions).
 * @func: A #SeedFunctionCallback to implement the function.
 * @obj: The #SeedObject on which to put the function.
 *
 * Creates a JavaScript object representing a first-class function; when
 * the function is called from JavaScript, @func will be called. Places
 * the created function as the property @name on @obj.
 *
 */
void
seed_create_function(JSContextRef ctx,
                     gchar* name,
                     gpointer func,
                     JSObjectRef obj)
{
    JSObjectRef oref;

    oref = JSObjectMakeFunctionWithCallback(ctx, NULL, func);
    seed_object_set_property(ctx, obj, name, oref);
}

void
seed_repl_expose(JSContextRef ctx, ...)
{
    va_list argp;
    void* expose;
    JSObjectRef arrayObj;
    guint i = 0;
    JSStringRef script;
    JSObjectRef seed
      = (JSObjectRef) seed_object_get_property(ctx,
                                               JSContextGetGlobalObject(ctx),
                                               "Seed");
    va_start(argp, ctx);

    arrayObj = JSObjectMake(ctx, NULL, NULL);

    g_print("Seed Debug REPL\n\nExposing:\n");

    while ((expose = va_arg(argp, void*) )) {
        g_print("  Seed.debug_argv[%u] = %p\n", i, expose);
        JSObjectSetPropertyAtIndex(ctx, arrayObj, i++, expose, NULL);
    }

    g_print("\n");

    seed_object_set_property(ctx, seed, "debug_argv", arrayObj);

    script = JSStringCreateWithUTF8CString(
      "readline = imports.readline;"
      "while(1) { try { print(eval("
      "readline.readline(\"> \"))); } catch(e) {"
      "print(e.name + \" \" + e.message);}}");

    JSEvaluateScript(ctx, script, NULL, NULL, 0, NULL);

    JSStringRelease(script);

    va_end(argp);
}

static void
seed_log_handler(const gchar* domain,
                 GLogLevelFlags log_level,
                 const gchar* message,
                 gpointer user_data)
{
    if (glib_message)
        g_free(glib_message);
    glib_message = g_strdup(message);
}

#ifdef SEED_ENABLE_DEBUG
static gboolean
seed_arg_debug_cb(const char* key, const char* value, gpointer user_data)
{
    seed_debug_flags |= g_parse_debug_string(value, seed_debug_keys,
                                             G_N_ELEMENTS(seed_debug_keys));
    return TRUE;
}

static gboolean
seed_arg_no_debug_cb(const char* key, const char* value, gpointer user_data)
{
    seed_debug_flags &= ~g_parse_debug_string(value, seed_debug_keys,
                                              G_N_ELEMENTS(seed_debug_keys));
    return TRUE;
}
#endif /* SEED_ENABLE_DEBUG */

static GOptionEntry seed_args[] = {
#ifdef SEED_ENABLE_DEBUG
    { "seed-debug", 0, 0, G_OPTION_ARG_CALLBACK, seed_arg_debug_cb,
      "Seed debugging messages to show. Comma separated list of: all, misc, "
      "finalization, initialization, construction, invocation, signal, "
      "structs, gtype.",
      "FLAGS" },
    { "seed-no-debug", 0, 0, G_OPTION_ARG_CALLBACK, seed_arg_no_debug_cb,
      "Disable Seed debugging", "FLAGS" },
#endif /* SEED_ENABLE_DEBUG */
    { "seed-version", 0, 0, G_OPTION_ARG_NONE, &seed_arg_print_version,
      "Print libseed version", 0 },
    {
      NULL,
    },
};

GOptionGroup*
seed_get_option_group(void)
{
    GOptionGroup* group;

    group = g_option_group_new("seed", "Seed Options", "Show Seed Options",
                               NULL, NULL);
    g_option_group_add_entries(group, seed_args);

    return group;
}

static gboolean
seed_parse_args(int* argc, char*** argv)
{
    GOptionContext* option_context;
    GOptionGroup* seed_group;

    GError* error = NULL;
    gboolean ret = TRUE;

    option_context = g_option_context_new(NULL);
    g_option_context_set_ignore_unknown_options(option_context, TRUE);
    g_option_context_set_help_enabled(option_context, TRUE);

    /* Initiate any command line options from the backend */

    seed_group = seed_get_option_group();
    g_option_context_add_group(option_context, seed_group);

    if (!g_option_context_parse(option_context, argc, argv, &error)) {
        if (error) {
            g_warning("%s", error->message);
            g_error_free(error);
        }

        ret = FALSE;
    }

    g_option_context_free(option_context);

    return ret;
}

void
seed_engine_destroy(SeedEngine* eng)
{
    JSValueUnprotect(eng->context, eng->global);
    JSGlobalContextRelease(eng->context);
    JSContextGroupRelease(eng->group);

    g_free(eng);
}

/**
 * seed_init_constrained_with_context_and_group:
 * @argc: A reference to the number of arguments remaining to parse.
 * @argv: A reference to an array of string arguments remaining to parse.
 * @context A reference to an existing JavascriptCore context
 * @group: A #SeedContextGroup within which to create the initial context.
 *
 * Initializes an empty new #SeedEngine. The Javascript context of this engine
 * is
 * *not* filled with any builtins functions and the import system.
 *
 * The javascript code executed with this engine can't import arbitrary
 * GObject introspected librairies.
 *
 * Use this when control over what is exposed in the Javascript context is
 * required,
 * for security concerns for example.
 * GObject instances can be selectively exposed by calling
 * @seed_engine_expose_gobject.
 * Namespaces can be selectively exposed by calling
 *
 * This function should only be called once within a single Seed application.
 *
 * Return value: The newly created and initialized #SeedEngine.
 *
 */
SeedEngine*
seed_init_constrained_with_context_and_group(gint* argc,
                                             gchar*** argv,
                                             JSGlobalContextRef context,
                                             JSContextGroupRef group)
{
#if !GLIB_CHECK_VERSION(2, 36, 0)
    g_type_init();
#endif
    g_log_set_handler("GLib-GObject", G_LOG_LEVEL_WARNING, seed_log_handler, 0);

    if ((argc != 0) && seed_parse_args(argc, argv) == FALSE) {
        SEED_NOTE(MISC, "failed to parse arguments.");
        return FALSE;
    }

    if (seed_arg_print_version) {
        g_print("%s\n", "Seed " VERSION);
        exit(EXIT_SUCCESS);
    }

    qname = g_quark_from_static_string("js-type");
    qprototype = g_quark_from_static_string("js-prototype");
    js_ref_quark = g_quark_from_static_string("js-ref");

    pthread_key_create(&seed_next_gobject_wrapper_key, NULL);

    eng = (SeedEngine*) g_malloc(sizeof(SeedEngine));

    context_group = group;

    eng->context = context;
    eng->global = JSContextGetGlobalObject(eng->context);
    eng->group = context_group;
    eng->search_path = NULL;

    function_proto
      = (JSObjectRef) seed_simple_evaluate(eng->context, "Function.prototype",
                                           NULL);

    gobject_class = JSClassCreate(&gobject_def);
    JSClassRetain(gobject_class);
    gobject_method_class = JSClassCreate(&gobject_method_def);
    JSClassRetain(gobject_method_class);
    gobject_constructor_class = JSClassCreate(&gobject_constructor_def);
    JSClassRetain(gobject_constructor_class);
    gobject_named_constructor_class
      = JSClassCreate(&gobject_named_constructor_def);
    JSClassRetain(gobject_named_constructor_class);
    gobject_signal_class = JSClassCreate(seed_get_signal_class());
    JSClassRetain(gobject_signal_class);
    seed_callback_class = JSClassCreate(&seed_callback_def);
    JSClassRetain(seed_callback_class);
    seed_struct_constructor_class = JSClassCreate(&struct_constructor_def);
    JSClassRetain(seed_struct_constructor_class);
    gobject_init_method_class = JSClassCreate(&gobject_init_method_def);
    JSClassRetain(gobject_init_method_class);

    g_type_set_qdata(G_TYPE_OBJECT, qname, gobject_class);

    seed_obj_ref = JSObjectMake(eng->context, NULL, NULL);
    seed_object_set_property(eng->context, eng->global, "Seed", seed_obj_ref);
    JSValueProtect(eng->context, seed_obj_ref);

    window_obj_ref = JSObjectMake(eng->context, NULL, NULL);
    seed_object_set_property(eng->context, eng->global, "window",
                             window_obj_ref);
    JSValueProtect(eng->context, window_obj_ref);
    g_irepository_require(g_irepository_get_default(), "GObject", NULL, 0, 0);
    g_irepository_require(g_irepository_get_default(), "GIRepository", NULL, 0,
                          0);

    seed_structs_init();
    seed_closures_init();

    return eng;
}

/**
 * seed_init_with_context_and_group:
 * @argc: A reference to the number of arguments remaining to parse.
 * @argv: A reference to an array of string arguments remaining to parse.
 * @context A reference to an existing JavascriptCore context
 * @group: A #SeedContextGroup within which to create the initial context.
 *
 * Initializes a new #SeedEngine using an existing JavascriptCore context.
 * This involves initializing GLib, adding @instance to @group, adding the
 * default globals to the provided context, and initializing various internal
 * parts of Seed.
 *
 * This function should only be called once within a single Seed application.
 *
 * Return value: The newly created and initialized #SeedEngine.
 *
 */
SeedEngine*
seed_init_with_context_and_group(gint* argc,
                                 gchar*** argv,
                                 JSGlobalContextRef context,
                                 JSContextGroupRef group)
{

    eng = seed_init_constrained_with_context_and_group(argc, argv, context,
                                                       group);
    seed_init_builtins(eng, argc, argv);
    seed_initialize_importer(eng->context, eng->global);
    seed_gtype_init(eng);

    defaults_script = JSStringCreateWithUTF8CString(
      "Seed.include(\"" SEED_PREFIX_PATH "extensions/Seed.js\");");

    JSEvaluateScript(eng->context, defaults_script, NULL, NULL, 0, NULL);

    base_info_info = g_irepository_find_by_name(0, "GIRepository", "BaseInfo");

    return eng;
}

/**
 * seed_init_with_context_group:
 * @argc: A reference to the number of arguments remaining to parse.
 * @argv: A reference to an array of string arguments remaining to parse.
 * @group: A #SeedContextGroup within which to create the initial context.
 *
 * Initializes a new #SeedEngine. This involves initializing GLib, creating
 * an initial context (in #group) with all of the default globals, and
 * initializing various internal parts of Seed.
 *
 * This function should only be called once within a single Seed application.
 *
 * Return value: The newly created and initialized #SeedEngine.
 *
 */
SeedEngine*
seed_init_with_context_group(gint* argc, gchar*** argv, JSContextGroupRef group)
{
    return seed_init_with_context_and_group(
      argc, argv, JSGlobalContextCreateInGroup(group, NULL), group);
}

/**
 * seed_init:
 * @argc: A reference to the number of arguments remaining to parse.
 * @argv: A reference to an array of string arguments remaining to parse.
 *
 * Initializes a new #SeedEngine. This involves initializing GLib, creating
 * an initial context with all of the default globals, and initializing
 * various internal parts of Seed.
 *
 * This function should only be called once within a single Seed application.
 *
 * Return value: The newly created and initialized #SeedEngine.
 *
 */
SeedEngine*
seed_init(gint* argc, gchar*** argv)
{
    context_group = JSContextGroupCreate();

    return seed_init_with_context_group(argc, argv, context_group);
}

/**
 * seed_init_with_context:
 * @argc: A reference to the number of arguments remaining to parse.
 * @argv: A reference to an array of string arguments remaining to parse.
 * @context A reference to an existing JavascriptCore context

 * Initializes a new #SeedEngine using an existing JavascriptCore context.
 * This involves initializing GLib, adding the default globals to the provided
 * @context and initializing various internal parts of Seed.
 *
 * This function should only be called once within a single Seed application.
 *
 * Return value: The newly created and initialized #SeedEngine.
 *
 */
SeedEngine*
seed_init_with_context(gint* argc, gchar*** argv, JSGlobalContextRef context)
{
    context_group = JSContextGetGroup(context);

    return seed_init_with_context_and_group(argc, argv, context, context_group);
}

/**
 * seed_init_constrained:
 * @argc: A reference to the number of arguments remaining to parse.
 * @argv: A reference to an array of string arguments remaining to parse.
 *
 * Initializes an empty new #SeedEngine. The Javascript context of this engine
 * is
 * *not* filled with any builtins functions and the import system.
 *
 * The javascript code executed with this engine can't import arbitrary
 * GObject introspected librairies.
 *
 * Use this when control over what is exposed in the Javascript context is
 * required,
 * for security concerns for example.
 * GObject instances can be selectively exposed by calling
 * @seed_engine_expose_gobject.
 * Namespaces can be selectively exposed by calling
 *
 * This function should only be called once within a single Seed application.
 *
 * Return value: The newly created and initialized #SeedEngine.
 *
 */
SeedEngine*
seed_init_constrained(gint* argc, gchar*** argv)
{
    context_group = JSContextGroupCreate();

    return seed_init_constrained_with_context_and_group(
      argc, argv, JSGlobalContextCreateInGroup(context_group, NULL),
      context_group);
}

/*
 * seed_engine_expose_gobject:
 * @engine:
 * @name: The name of the global javascript variable pointing to @object
 * @object: The #GObject instance that will be exposed in the the javascript
 * context
 * @gir_namespace: The Introspection namespace containing the type of the
 * provided
 * object.
 *
 * Expose a GObject instance to the global Javascript context and makes it
 * accessible
 * under the provided @js_name
 *
 * return: the SeedValue representing @object, NULL in case of error
 */
JSValueRef
seed_engine_expose_gobject(SeedEngine* engine,
                           gchar* js_name,
                           GObject* object,
                           gchar* gir_namespace,
                           JSValueRef* exception)
{
    GError* error = NULL;

    g_assert(engine != NULL && gir_namespace != NULL && js_name != NULL);

    GITypelib* type_lib = g_irepository_require(g_irepository_get_default(),
                                                gir_namespace, NULL, 0, &error);
    if (type_lib == NULL) {
        seed_make_exception_from_gerror(engine->context, exception, error);
        g_error_free(error);
        return NULL;
    }

    JSValueRef obj_js_value
      = seed_value_from_object(engine->context, G_OBJECT(object), exception);
    g_return_val_if_fail(obj_js_value != NULL, NULL);

    g_return_val_if_fail(engine->global != NULL, NULL);

    gboolean ok = seed_object_set_property(engine->context, engine->global,
                                           js_name, obj_js_value);
    g_return_val_if_fail(ok == TRUE, NULL);

    return obj_js_value;
}

/*
 * seed_engine_expose_namespace:
 * @engine:
 * @namespace: Name of the GIR Namespace that should be exposed in the JS
 * context.
 *
 * Expose a GIR namespace in the global Javascript context and makes it
 * accessible
 * under a variable named after the namespace (ex: the namespace 'Notify' is
 * held
 * by the 'Notify' javascript variable.
 *
 * return: the SeedValue representing @namespace in the javascript context,
 *  NULL in case of error
 */
JSValueRef
seed_engine_expose_namespace(SeedEngine* engine,
                             gchar* namespace_name,
                             JSValueRef* exception)
{
    g_assert(engine != NULL && namespace_name != NULL);

    JSValueRef namespace
      = seed_gi_importer_do_namespace(engine->context, namespace_name,
                                      exception);
    g_return_val_if_fail(namespace != NULL, NULL);
    gboolean ok = seed_object_set_property(engine->context, engine->global,
                                           namespace_name, namespace);
    g_return_val_if_fail(ok == TRUE, FALSE);

    return namespace;
}
