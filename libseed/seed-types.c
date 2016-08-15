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
#include <dlfcn.h>
#include <pthread.h>

JSClassRef gobject_class;
JSClassRef gobject_method_class;
JSClassRef gobject_constructor_class;
JSClassRef seed_callback_class;
JSClassRef gobject_init_method_class;
SeedEngine* eng;

static gboolean
seed_value_is_gobject(JSContextRef ctx, JSValueRef value)
{
    if (!JSValueIsObject(ctx, value) || JSValueIsNull(ctx, value))
        return FALSE;

    return JSValueIsObjectOfClass(ctx, value, gobject_class);
}

void
seed_toggle_ref(gpointer data, GObject* object, gboolean is_last_ref)
{
    JSValueRef wrapper;

    if (!g_object_get_qdata(object, js_ref_quark))
        return;

    wrapper = (JSValueRef) data;

    if (is_last_ref) {
        JSValueUnprotect(eng->context, wrapper);
    } else {
        JSValueProtect(eng->context, wrapper);
    }
}

static void
seed_gobject_destroyed(gpointer object)
{
    JSValueUnprotect(eng->context, (JSValueRef) object);
    JSObjectSetPrivate((JSObjectRef) object, 0);
}

JSObjectRef
seed_make_wrapper_for_type(JSContextRef ctx, GType type)
{
    JSClassRef class;
    JSObjectRef ret;
    JSValueRef prototype;

    class = seed_gobject_get_class_for_gtype(ctx, type);

    while (!class && (type = g_type_parent(type)))
        class = seed_gobject_get_class_for_gtype(ctx, type);

    prototype = seed_gobject_get_prototype_for_gtype(type);
    ret = JSObjectMake(ctx, class, NULL);
    if (prototype)
        JSObjectSetPrototype(ctx, ret, prototype);
    else
        g_assert_not_reached();

    return ret;
}

static JSValueRef
seed_wrap_object(JSContextRef ctx, GObject* object)
{
    JSValueRef user_data;
    JSObjectRef js_ref;
    GType type;

    type = G_OBJECT_TYPE(object);

    user_data = (JSValueRef) g_object_get_qdata(object, js_ref_quark);

    if (user_data)
        return user_data;

    if (pthread_getspecific(seed_next_gobject_wrapper_key))
        js_ref = pthread_getspecific(seed_next_gobject_wrapper_key);
    else
        js_ref = seed_make_wrapper_for_type(ctx, type);

    JSObjectSetPrivate(js_ref, object);

    g_object_set_qdata_full(object, js_ref_quark, (gpointer) js_ref,
                            seed_gobject_destroyed);

    JSValueProtect(eng->context, js_ref);
    g_object_add_toggle_ref(object, seed_toggle_ref, (gpointer) js_ref);

    seed_add_signals_to_object(ctx, js_ref, object);

    pthread_setspecific(seed_next_gobject_wrapper_key, NULL);

    return js_ref;
}

static gboolean
seed_release_arg(GITransfer transfer,
                 GITypeInfo* type_info,
                 GITypeTag type_tag,
                 GArgument* arg)
{
    GType gtype;
    GITypeInfo* param_type;
    GIBaseInfo* interface_info;
    GValue* gval;

    switch (type_tag) {
        case GI_TYPE_TAG_UTF8:
        case GI_TYPE_TAG_FILENAME:
            g_free(arg->v_string);
            break;
        case GI_TYPE_TAG_ARRAY:
            if (arg->v_pointer) {
                param_type = g_type_info_get_param_type(type_info, 0);

                switch (g_type_info_get_tag(param_type)) {
                    case GI_TYPE_TAG_UTF8:
                        if (transfer == GI_TRANSFER_EVERYTHING)
                            g_strfreev(arg->v_pointer);
                        else if (transfer == GI_TRANSFER_CONTAINER)
                            g_free(arg->v_pointer);
                        break;
                    case GI_TYPE_TAG_GTYPE:
                    case GI_TYPE_TAG_FLOAT:
                    case GI_TYPE_TAG_UINT8:
                    case GI_TYPE_TAG_INT32:
                    case GI_TYPE_TAG_UINT32:
                        g_free(arg->v_pointer);
                        break;
                    case GI_TYPE_TAG_INTERFACE:
                        break;
                    default:
                        g_assert_not_reached();
                }

                g_base_info_unref((GIBaseInfo*) param_type);
            }
            break;
        case GI_TYPE_TAG_GHASH: {
            GITypeInfo* val_param_info;
            GHashTableIter iter;
            GArgument value;

            val_param_info = g_type_info_get_param_type(type_info, 1);
            g_assert(val_param_info != NULL);

            /* release the hashtable's values */
            g_hash_table_iter_init(&iter, arg->v_pointer);
            while (g_hash_table_iter_next(&iter, NULL, &value.v_pointer)) {
                seed_release_arg(GI_TRANSFER_EVERYTHING, val_param_info,
                                 g_type_info_get_tag(val_param_info), &value);
            }

            /* release the hashtable's keys and the hashtable itself */
            g_hash_table_destroy(arg->v_pointer);
            break;
        }
        case GI_TYPE_TAG_INTERFACE: {
            if (arg->v_pointer) {
                interface_info = g_type_info_get_interface(type_info);

                GIInfoType interface_type
                  = g_base_info_get_type(interface_info);
                if (interface_type == GI_INFO_TYPE_CALLBACK) {
                    /* FIXME: - callback returns are not handled yet */
                    g_base_info_unref(interface_info);
                    break;
                }

                gtype = g_registered_type_info_get_g_type(
                  (GIRegisteredTypeInfo*) interface_info);

                if (g_type_is_a(gtype, G_TYPE_OBJECT)
                    || g_type_is_a(gtype, G_TYPE_INTERFACE)) {
                    SEED_NOTE(MISC, "Unreffing object of type: %s in"
                                    "argument release. Reference count: %d\n",
                              g_type_name(
                                G_OBJECT_TYPE(G_OBJECT(arg->v_pointer))),
                              G_OBJECT(arg->v_pointer)->ref_count);
                    g_object_unref(G_OBJECT(arg->v_pointer));
                } else if (g_type_is_a(gtype, G_TYPE_VALUE)) {
                    gval = (GValue*) arg->v_pointer;
                    // Free/unref the GValue's contents.
                    g_value_unset(gval);
                    // Free the GValue.
                    g_slice_free1(sizeof(GValue), gval);
                } else if (g_type_is_a(gtype, G_TYPE_CLOSURE)) {
                    g_closure_unref(arg->v_pointer);
                }

                g_base_info_unref(interface_info);
            }
            break;
        }
        default:
            break;
    }

    return TRUE;
}

gboolean
seed_gi_release_arg(GITransfer transfer, GITypeInfo* type_info, GArgument* arg)
{
    GITypeTag type_tag;

    if (transfer == GI_TRANSFER_NOTHING)
        return TRUE;

    type_tag = g_type_info_get_tag((GITypeInfo*) type_info);

    return seed_release_arg(transfer, type_info, type_tag, arg);
}

gboolean
seed_gi_release_in_arg(GITransfer transfer,
                       GITypeInfo* type_info,
                       GArgument* arg)
{
    GITypeTag type_tag;

    if (transfer == GI_TRANSFER_EVERYTHING)
        return TRUE;

    type_tag = g_type_info_get_tag((GITypeInfo*) type_info);

    switch (type_tag) {
        // TODO: FIXME: Leaaaks?
        case GI_TYPE_TAG_INTERFACE: {
            // TODO: FIXME: Need some safe way to look for GClosure.
            break;
        }
        case GI_TYPE_TAG_UTF8:
        case GI_TYPE_TAG_FILENAME:
        case GI_TYPE_TAG_ARRAY:
        case GI_TYPE_TAG_GHASH:
            return seed_release_arg(GI_TRANSFER_EVERYTHING, type_info, type_tag,
                                    arg);
        default:
            break;
    }

    return TRUE;
}

static JSValueRef
seed_gi_make_jsarray(JSContextRef ctx,
                     void* array,
                     GITypeInfo* param_type,
                     JSValueRef* exception)
{
    GITypeTag element_type;
    JSValueRef* elements;
    guint length, i;
    gchar** str_array = (gchar**) array;
    JSValueRef ret = JSValueMakeNull(ctx);

    element_type = g_type_info_get_tag(param_type);

    if (element_type == GI_TYPE_TAG_UTF8) {

        length = g_strv_length(str_array);
        if (!length)
            return ret;

        elements = g_alloca(sizeof(JSValueRef) * length);
        for (i = 0; i < length; ++i) {
            elements[i] = seed_value_from_string(ctx, str_array[i], exception);
        }

        ret = (JSValueRef) JSObjectMakeArray(ctx, length, elements, exception);
    }

    if (element_type == GI_TYPE_TAG_GTYPE) {
        GType* ptr = (GType*) array;
        length = 0;
        while (ptr[length])
            length++;

        elements = g_alloca(sizeof(JSValueRef) * length);

        for (i = 0; i < length; ++i)
            elements[i] = seed_value_from_long(ctx, ptr[i], exception);

        return (JSValueRef) JSObjectMakeArray(ctx, length, elements, exception);
    }

    return ret;
}

JSValueRef
seed_array_from_zero_terminated_c_array(JSContextRef ctx,
                                        gpointer pointer,
                                        GITypeInfo* param_info,
                                        JSValueRef* exception)
{
    JSValueRef ret;
    GITypeTag element_type;
    GArgument arg;
    guint i;

    element_type = g_type_info_get_tag(param_info);

    // Special case handling array(guint8), which happens to be a string
    // in most cases
    if (element_type == GI_TYPE_TAG_UINT8) {
        // TODO: this is pretty simple and probably incomplete.
        // GJS makes possible the return of an array, supporting
        // all types. We're only supporting strings ATM.
        ret = seed_value_from_string(ctx, pointer, exception);
        return ret;
    }

    ret = JSObjectMakeArray(ctx, 0, NULL, NULL);

#define ITERATE(type)                                                          \
    do {                                                                       \
        g##type* array = (g##type*) pointer;                                   \
        for (i = 0; array[i]; i++) {                                           \
            arg.v_##type = array[i];                                           \
            JSValueRef val                                                     \
              = seed_value_from_gi_argument(ctx, &arg, param_info, exception); \
            seed_object_set_property_at_index(ctx, (JSObjectRef) ret, i, val, exception);    \
        }                                                                      \
    } while (0);

    switch (element_type) {
        /* We handle GI_TYPE_TAG_UINT8 above. */
        case GI_TYPE_TAG_INT8:
            ITERATE(int8);
            break;
        case GI_TYPE_TAG_UINT16:
            ITERATE(uint16);
            break;
        case GI_TYPE_TAG_INT16:
            ITERATE(int16);
            break;
        case GI_TYPE_TAG_UINT32:
            ITERATE(uint32);
            break;
        case GI_TYPE_TAG_INT32:
            ITERATE(int32);
            break;
        case GI_TYPE_TAG_UINT64:
            ITERATE(uint64);
            break;
        case GI_TYPE_TAG_INT64:
            ITERATE(int64);
            break;
        case GI_TYPE_TAG_FLOAT:
            ITERATE(float);
            break;
        case GI_TYPE_TAG_DOUBLE:
            ITERATE(double);
            break;
        case GI_TYPE_TAG_GTYPE:
        case GI_TYPE_TAG_UTF8:
        case GI_TYPE_TAG_FILENAME:
        case GI_TYPE_TAG_ARRAY:
        case GI_TYPE_TAG_INTERFACE:
        case GI_TYPE_TAG_GLIST:
        case GI_TYPE_TAG_GSLIST:
        case GI_TYPE_TAG_GHASH:
        case GI_TYPE_TAG_ERROR:
            ITERATE(pointer);
            break;
        default:
            seed_make_exception(ctx, exception, "Argumenterror",
                                "Unknown element-type %d", element_type);
    }

#undef ITERATE

    return ret;
}

static gboolean
seed_gi_make_array_from_string(JSContextRef ctx,
                               JSStringRef js_string,
                               GITypeInfo* param_type,
                               guint length,
                               void** array_p,
                               guint* out_array_length,
                               JSValueRef* exception)
{
    GITypeTag element_type;
    element_type = g_type_info_get_tag(param_type);

    // This could be handled by the case where the value is an object,
    // however, getting length from a string crashs inside JSC. So,
    // we're now proper handling strings here
    gchar* buffer = g_malloc(length * sizeof(gchar));
    guint real_size = JSStringGetUTF8CString(js_string, buffer, length);

    switch (element_type) {
        case GI_TYPE_TAG_INT8:
        case GI_TYPE_TAG_UINT8: {
            *array_p = buffer;

            // So, GJS doesn't computer the \0 at the end of line.
            // it does make sense, as converting a string to an array
            // we don't want the EOS. However, I couldn't find a way to do it
            // with
            // JSC, so I'm basically reducing one byte.
            if (out_array_length)
                *out_array_length = real_size - 1;
            break;
        }
        case GI_TYPE_TAG_INT16:
        case GI_TYPE_TAG_UINT16:
        // TODO: implement utf16 support
        default: {
            seed_make_exception(ctx, exception, "ArgumentError",
                                "Cannot convert string to array of '%s'",
                                g_type_tag_to_string(element_type));
            return FALSE;
        }
    }

    return TRUE;
}

static gboolean
seed_gi_make_array(JSContextRef ctx,
                   JSValueRef array,
                   guint length,
                   GITypeInfo* param_type,
                   void** array_p,
                   JSValueRef* exception)
{
    GITypeTag element_type;
    JSValueRef elem;
    guint i;

    element_type = g_type_info_get_tag(param_type);

    switch (element_type) {
        case GI_TYPE_TAG_UTF8: {
            gchar** strresult = g_new0(gchar*, length + 1);

            for (i = 0; i < length; i++) {
                elem = JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) array, i,
                                                  exception);
                strresult[i] = seed_value_to_string(ctx, elem, exception);
            }

            *array_p = strresult;
        } break;
        case GI_TYPE_TAG_GTYPE: {
            GType* typeresult;

            typeresult = g_new0(GType, length + 1);

            for (i = 0; i < length; i++) {
                elem = JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) array, i,
                                                  exception);
                typeresult[i] = seed_value_to_int(ctx, elem, exception);
            }

            *array_p = typeresult;
        } break;
        case GI_TYPE_TAG_FLOAT: {
            gfloat* floatresult;

            floatresult = g_new0(gfloat, length + 1);

            for (i = 0; i < length; i++) {
                elem = JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) array, i,
                                                  exception);
                floatresult[i] = seed_value_to_float(ctx, elem, exception);
            }

            *array_p = floatresult;
        } break;
        case GI_TYPE_TAG_DOUBLE: {
            gdouble* dblresult;

            dblresult = g_new0(gdouble, length + 1);

            for (i = 0; i < length; i++) {
                elem = JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) array, i,
                                                  exception);
                dblresult[i] = seed_value_to_double(ctx, elem, exception);
            }

            *array_p = dblresult;
        } break;

#if GOBJECT_INTROSPECTION_VERSION < 0x000900
        case GI_TYPE_TAG_INT:
#endif
        case GI_TYPE_TAG_INT32: {
            gint* intresult;

            intresult = g_new0(gint, length + 1);

            for (i = 0; i < length; i++) {
                elem = JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) array, i,
                                                  exception);
                intresult[i] = seed_value_to_int(ctx, elem, exception);
            }

            *array_p = intresult;
        } break;
        case GI_TYPE_TAG_UINT8: {
            guint8* guint8result;

            guint8result = g_new0(guint8, length + 1);

            for (i = 0; i < length; i++) {
                elem = JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) array, i,
                                                  exception);
                guint8result[i] = seed_value_to_uchar(ctx, elem, exception);
            }

            *array_p = guint8result;
        } break;
        case GI_TYPE_TAG_INTERFACE: {
            GIBaseInfo* interface = g_type_info_get_interface(param_type);
            GIInfoType interface_type = g_base_info_get_type(interface);
            if (interface_type == GI_INFO_TYPE_OBJECT
                || interface_type == GI_INFO_TYPE_INTERFACE
                || interface_type == GI_INFO_TYPE_STRUCT) {
                GType type = g_registered_type_info_get_g_type(
                  (GIRegisteredTypeInfo*) interface);
                if (g_type_is_a(type, G_TYPE_VALUE)) {
                    GValue* gvalresult;

                    // TODO:FIXME: Robb. Valgrind thinks there's a leak here,
                    //             at least while running Same Seed.
                    gvalresult = g_new0(GValue, length + 1);

                    for (i = 0; i < length; i++) {
                        elem
                          = JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) array,
                                                       i, exception);
                        seed_value_to_gvalue(ctx, elem, (GType) 0,
                                             &gvalresult[i], exception);
                    }
                    *array_p = gvalresult;

                    g_base_info_unref(interface);
                    break;
                }
            }

            g_base_info_unref(interface);
        }
        case GI_TYPE_TAG_UINT32: {
            guint32* uintresult;

            uintresult = g_new0(guint32, length + 1);

            for (i = 0; i < length; i++) {
                elem = JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) array, i,
                                                  exception);
                uintresult[i] = seed_value_to_uint(ctx, elem, exception);
            }
            *array_p = uintresult;
        } break;
        default:
            seed_make_exception(ctx, exception, "ArgumentError",
                                "Unhandled array element type");
            return FALSE;
    }

    return TRUE;
}

gboolean
seed_value_to_gi_argument_with_out_length(JSContextRef ctx,
                                          JSValueRef value,
                                          GITypeInfo* type_info,
                                          GITransfer transfer,
                                          GArgument* arg,
                                          guint* out_array_length,
                                          JSValueRef* exception)
{
    GITypeTag gi_tag = g_type_info_get_tag(type_info);

    // FIXME: Some types are not "nullable", also need to check if argument
    // can be null before doing this.
    if (!value || JSValueIsNull(ctx, value)) {
        arg->v_pointer = 0;
        return 1;
    }

    switch (gi_tag) {
        case GI_TYPE_TAG_VOID:

            if (g_type_info_is_pointer(type_info)) {
                GObject* gobject;
                if (JSValueIsString(ctx, value)) {
                    /*
                      things like gio.outputstream.write use void pointers
                      might need a few other types here..
                      not very well memory managed. - should be solved by
                      bytearrays when
                      introspection implements it.
                    */
                    arg->v_string = seed_value_to_string(ctx, value, exception);
                    break;
                }
                /* gtk_statusicon_position_menu / gtk_menu_popup use the
                 * userdata for the Gobject */
                gobject = seed_value_to_object(ctx, value, exception);
                if (!gobject)
                    return FALSE;

                arg->v_pointer = gobject;
            }
            break;

#if GOBJECT_INTROSPECTION_VERSION < 0x000900
        case GI_TYPE_TAG_LONG:
            arg->v_long = seed_value_to_long(ctx, value, exception);
            break;
        case GI_TYPE_TAG_ULONG:
            arg->v_ulong = seed_value_to_ulong(ctx, value, exception);
            break;
        case GI_TYPE_TAG_INT:
            arg->v_int = seed_value_to_int(ctx, value, exception);
            break;
        case GI_TYPE_TAG_UINT:
            arg->v_uint = seed_value_to_uint(ctx, value, exception);
            break;
        case GI_TYPE_TAG_SIZE:
            arg->v_size = seed_value_to_size(ctx, value, exception);
            break;
        case GI_TYPE_TAG_SSIZE:
            arg->v_ssize = seed_value_to_ssize(ctx, value, exception);
            break;
        case GI_TYPE_TAG_TIME_T:
            arg->v_long = seed_value_to_time_t(ctx, value, exception);
            break;
#endif

        case GI_TYPE_TAG_BOOLEAN:
            arg->v_boolean = seed_value_to_boolean(ctx, value, exception);
            break;
        case GI_TYPE_TAG_INT8:
            arg->v_int8 = seed_value_to_char(ctx, value, exception);
            break;
        case GI_TYPE_TAG_UINT8:
            arg->v_uint8 = seed_value_to_uchar(ctx, value, exception);
            break;
        case GI_TYPE_TAG_INT16:
            arg->v_int16 = seed_value_to_int(ctx, value, exception);
            break;
        case GI_TYPE_TAG_UINT16:
            arg->v_uint16 = seed_value_to_uint(ctx, value, exception);
            break;
        case GI_TYPE_TAG_INT32:
            arg->v_int32 = seed_value_to_int(ctx, value, exception);
            break;
        case GI_TYPE_TAG_UINT32:
            arg->v_uint32 = seed_value_to_uint(ctx, value, exception);
            break;
        case GI_TYPE_TAG_INT64:
            arg->v_int64 = seed_value_to_int64(ctx, value, exception);
            break;
        case GI_TYPE_TAG_UINT64:
            arg->v_uint64 = seed_value_to_uint64(ctx, value, exception);
            break;
        case GI_TYPE_TAG_FLOAT:
            arg->v_float = seed_value_to_float(ctx, value, exception);
            break;
        case GI_TYPE_TAG_DOUBLE:
            arg->v_double = seed_value_to_double(ctx, value, exception);
            break;
        case GI_TYPE_TAG_UTF8:
            arg->v_string = seed_value_to_string(ctx, value, exception);
            break;
        case GI_TYPE_TAG_FILENAME:
            arg->v_string = seed_value_to_filename(ctx, value, exception);
            break;
        case GI_TYPE_TAG_GTYPE:
            arg->v_int = seed_value_to_int(ctx, value, exception);
            break;
        case GI_TYPE_TAG_INTERFACE: {
            GIBaseInfo* interface;
            GIInfoType interface_type;
            GType required_gtype;
            GObject* gobject;

            interface = g_type_info_get_interface(type_info);
            interface_type = g_base_info_get_type(interface);

            arg->v_pointer = NULL;

            if (interface_type == GI_INFO_TYPE_OBJECT
                || interface_type == GI_INFO_TYPE_INTERFACE) {
                gobject = seed_value_to_object(ctx, value, exception);
                required_gtype = g_registered_type_info_get_g_type(
                  (GIRegisteredTypeInfo*) interface);

                // FIXME: Not clear if the g_type_is_a check is desired here.
                // Possibly 'breaks things' when we don't have introspection
                // data for some things in an interface hierarchy. Hasn't
                // provided any problems so far.
                if (!gobject
                    || !g_type_is_a(G_OBJECT_TYPE(gobject), required_gtype)) {
                    g_base_info_unref(interface);
                    return FALSE;
                }

                arg->v_pointer = gobject;
                // FIXME: This has to be done for other types too
                if (transfer == GI_TRANSFER_EVERYTHING)
                    g_object_ref(gobject);
                g_base_info_unref(interface);
                break;
            } else if (interface_type == GI_INFO_TYPE_ENUM
                       || interface_type == GI_INFO_TYPE_FLAGS) {
                // this might need fixing...
                arg->v_long = seed_value_to_long(ctx, value, exception);
                if (!(interface_type == GI_INFO_TYPE_FLAGS)
                    && !seed_validate_enum((GIEnumInfo*) interface,
                                           arg->v_long)) {
                    seed_make_exception(ctx, exception, "EnumRange",
                                        "Enum value: %ld is out of range",
                                        arg->v_long);
                    g_base_info_unref(interface);

                    return FALSE;
                }

                g_base_info_unref(interface);
                break;
            } else if (interface_type == GI_INFO_TYPE_STRUCT
                       || interface_type == GI_INFO_TYPE_UNION) {
                if (JSValueIsObjectOfClass(ctx, value, seed_struct_class))
                    arg->v_pointer = seed_pointer_get_pointer(ctx, value);
                else if (JSValueIsObjectOfClass(ctx, value, seed_union_class))
                    arg->v_pointer = seed_pointer_get_pointer(ctx, value);
                else {
                    GType type = g_registered_type_info_get_g_type(
                      (GIRegisteredTypeInfo*) interface);
                    if (!type) {
                        g_base_info_unref(interface);
                        return FALSE;
                    } else if (type == G_TYPE_VALUE) {
                        GValue* gval = g_slice_alloc0(sizeof(GValue));
                        seed_value_to_gvalue(ctx, value, (GType) NULL, gval,
                                             exception);
                        arg->v_pointer = gval;

                        g_base_info_unref(interface);
                        break;
                    }
                    // Automatically convert between functions and
                    // GClosures where expected.
                    else if (g_type_is_a(type, G_TYPE_CLOSURE)) {
                        if (JSObjectIsFunction(ctx, (JSObjectRef) value)) {
                            arg->v_pointer
                              = seed_closure_new(ctx, (JSObjectRef) value, NULL,
                                                 NULL);
                        }
                    } else {
                        JSObjectRef strukt
                          = seed_construct_struct_type_with_parameters(
                            ctx, interface, (JSObjectRef) value, exception);
                        arg->v_pointer = seed_pointer_get_pointer(ctx, strukt);
                    }
                }
                g_base_info_unref(interface);
                break;
            } else if (interface_type == GI_INFO_TYPE_CALLBACK) {
                if (JSValueIsNull(ctx, value)) {
                    arg->v_pointer = NULL;
                    g_base_info_unref(interface);
                    break;
                }
                // Someone passes in a wrapper around a method where a
                // callback is expected, i.e Clutter.sine_inc_func, as an alpha
                // Have to dlsym the symbol to be able to do this.
                // NOTE: Some cases where dlsym(NULL, symbol) doesn't work
                // depending
                // On how libseed is loaded.
                else if (JSValueIsObjectOfClass(ctx, value,
                                                gobject_method_class)) {
                    GIFunctionInfo* info
                      = JSObjectGetPrivate((JSObjectRef) value);
                    const gchar* symbol = g_function_info_get_symbol(info);
                    gchar* error;
                    void* fp;

                    dlerror();
                    fp = (void*) dlsym(0, symbol);
                    if ((error = dlerror()) != NULL) {
                        g_critical("dlerror: %s \n", error);
                    } else {
                        arg->v_pointer = fp;
                        g_base_info_unref(interface);
                        break;
                    }
                }
                // Somewhat deprecated from when it was necessary to manually
                // create closure objects...
                else if (JSValueIsObjectOfClass(ctx, value,
                                                seed_native_callback_class)) {
                    SeedNativeClosure* privates
                      = (SeedNativeClosure*) JSObjectGetPrivate(
                        (JSObjectRef) value);
                    arg->v_pointer = privates->closure;
                    g_base_info_unref(interface);
                    break;
                }
                // Automagically create closure around function passed in as
                // callback.
                else if (JSObjectIsFunction(ctx, (JSObjectRef) value)) {
                    SeedNativeClosure* privates
                      = seed_make_native_closure(ctx,
                                                 (GICallableInfo*) interface,
                                                 value);
                    arg->v_pointer = privates->closure;
                    g_base_info_unref(interface);
                    break;
                }
            }
        }
        case GI_TYPE_TAG_ARRAY: {
            if (JSValueIsNull(ctx, value)) {
                arg->v_pointer = NULL;
                break;
            } else if (JSValueIsString(ctx, value)) {
                GITypeInfo* param_type;
                param_type = g_type_info_get_param_type(type_info, 0);

                JSStringRef js_string
                  = JSValueToStringCopy(ctx, value, exception);
                size_t length = JSStringGetMaximumUTF8CStringSize(js_string);
                if (!seed_gi_make_array_from_string(ctx, js_string, param_type,
                                                    length, &arg->v_pointer,
                                                    out_array_length,
                                                    exception)) {
                    g_base_info_unref((GIBaseInfo*) param_type);
                    JSStringRelease(js_string);
                    return FALSE;
                }
                JSStringRelease(js_string);
                g_base_info_unref((GIBaseInfo*) param_type);
                break;
            } else if (!JSValueIsObject(ctx, value)) {
                // TODO: FIXME: Is this right?
                return FALSE;
            } else {
                GITypeInfo* param_type;
                // TODO: FIXME: Better array test like the cool one on reddit.
                guint length = seed_value_to_int(
                  ctx,
                  seed_object_get_property(ctx, (JSObjectRef) value, "length"),
                  exception);
                if (!length) {
                    arg->v_pointer = NULL;
                    break;
                }

                param_type = g_type_info_get_param_type(type_info, 0);
                if (!seed_gi_make_array(ctx, value, length, param_type,
                                        &arg->v_pointer, exception)) {
                    g_base_info_unref((GIBaseInfo*) param_type);
                    return FALSE;
                }
                if (out_array_length != NULL)
                    *out_array_length = length;

                g_base_info_unref((GIBaseInfo*) param_type);
                break;
            }
        }
        case GI_TYPE_TAG_GHASH: {
            GITypeInfo *key_param_info, *val_param_info;
            GHashTable* hash_table;

            JSPropertyNameArrayRef jsprops = 0;
            JSStringRef jsprop_name;
            JSValueRef jsprop_value;
            int i, nparams = 0;

            if (!JSValueIsObject(ctx, value)) {
                return FALSE;
            }

            if (JSValueIsNull(ctx, value)) {
                arg->v_pointer = NULL;
                break;
            }

            key_param_info = g_type_info_get_param_type(type_info, 0);
            g_assert(key_param_info != NULL);
            if (g_type_info_get_tag(key_param_info) != GI_TYPE_TAG_UTF8) {
                /* Functions requesting a hash table with non-string keys are
                 * not supported */
                return FALSE;
            }

            val_param_info = g_type_info_get_param_type(type_info, 1);
            g_assert(val_param_info != NULL);

            jsprops = JSObjectCopyPropertyNames(ctx, (JSObjectRef) value);
            nparams = JSPropertyNameArrayGetCount(jsprops);

            /* keys are strings and the destructor is g_free
             * values will be freed in seed_release_arg
             */
            hash_table
              = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

            for (i = 0; i < nparams; i++) {
                char* prop_name;
                int length;
                GArgument hash_arg;

                jsprop_name = JSPropertyNameArrayGetNameAtIndex(jsprops, i);

                length = JSStringGetMaximumUTF8CStringSize(jsprop_name);
                prop_name = g_malloc(length * sizeof(gchar));
                JSStringGetUTF8CString(jsprop_name, prop_name, length);

                jsprop_value = JSObjectGetProperty(ctx, (JSObjectRef) value,
                                                   jsprop_name, NULL);

                if (!seed_value_to_gi_argument(ctx, jsprop_value,
                                               val_param_info,
                                               GI_TRANSFER_NOTHING, &hash_arg,
                                               exception)) {
                    g_hash_table_destroy(hash_table);
                    g_base_info_unref((GIBaseInfo*) val_param_info);
                    return FALSE;
                }

                g_hash_table_insert(hash_table, prop_name, hash_arg.v_pointer);
            }

            arg->v_pointer = hash_table;

            break;
        }
        default:
            return FALSE;
    }
    return TRUE;
}

gboolean
seed_value_to_gi_argument(JSContextRef ctx,
                          JSValueRef value,
                          GITypeInfo* type_info,
                          GITransfer transfer,
                          GArgument* arg,
                          JSValueRef* exception)
{
    return seed_value_to_gi_argument_with_out_length(ctx, value, type_info,
                                                     transfer, arg, NULL,
                                                     exception);
}

JSValueRef
seed_value_from_gi_argument(JSContextRef ctx,
                            GArgument* arg,
                            GITypeInfo* type_info,
                            JSValueRef* exception)
{

    return seed_value_from_gi_argument_full(ctx, arg, type_info, exception, 0,
                                            g_type_info_get_tag(type_info));
}

JSValueRef
seed_value_from_gi_argument_full(JSContextRef ctx,
                                 GArgument* arg,
                                 GITypeInfo* type_info,
                                 JSValueRef* exception,
                                 guint64 array_len,
                                 GITypeTag gi_tag)
{

    // seed_value_from_gi_argument_with_length : g_type_tag_to_string(gi_tag)
    SEED_NOTE(INVOCATION, "seed_value_from_gi_argument_full: arg_type = %s ",
              g_type_tag_to_string(gi_tag));

    switch (gi_tag) {

#if GOBJECT_INTROSPECTION_VERSION < 0x000900
        case GI_TYPE_TAG_LONG:
            return seed_value_from_long(ctx, arg->v_long, exception);
        case GI_TYPE_TAG_ULONG:
            return seed_value_from_ulong(ctx, arg->v_ulong, exception);
        case GI_TYPE_TAG_INT:
            return seed_value_from_int(ctx, arg->v_int32, exception);
        case GI_TYPE_TAG_UINT:
            return seed_value_from_uint(ctx, arg->v_uint32, exception);
        case GI_TYPE_TAG_SSIZE:
            return seed_value_from_ssize(ctx, arg->v_ssize, exception);
        case GI_TYPE_TAG_SIZE:
            return seed_value_from_size(ctx, arg->v_size, exception);
        case GI_TYPE_TAG_TIME_T:
            return seed_value_from_time_t(ctx, arg->v_long, exception);
#endif
        case GI_TYPE_TAG_VOID:
            return JSValueMakeUndefined(ctx);
        case GI_TYPE_TAG_BOOLEAN:
            return seed_value_from_boolean(ctx, arg->v_boolean, exception);
        case GI_TYPE_TAG_INT8:
            return seed_value_from_char(ctx, arg->v_int8, exception);
        case GI_TYPE_TAG_UINT8:
            return seed_value_from_uchar(ctx, arg->v_uint8, exception);
        case GI_TYPE_TAG_INT16:
            return seed_value_from_int(ctx, arg->v_int16, exception);
        case GI_TYPE_TAG_UINT16:
            return seed_value_from_uint(ctx, arg->v_uint16, exception);
        case GI_TYPE_TAG_INT32:
            return seed_value_from_int(ctx, arg->v_int32, exception);
        case GI_TYPE_TAG_UINT32:
            return seed_value_from_uint(ctx, arg->v_uint32, exception);
        case GI_TYPE_TAG_INT64:
            return seed_value_from_int64(ctx, arg->v_int64, exception);
        case GI_TYPE_TAG_UINT64:
            return seed_value_from_uint64(ctx, arg->v_uint64, exception);
        case GI_TYPE_TAG_FLOAT:
            return seed_value_from_float(ctx, arg->v_float, exception);
        case GI_TYPE_TAG_DOUBLE:
            return seed_value_from_double(ctx, arg->v_double, exception);
        case GI_TYPE_TAG_UTF8:
            return seed_value_from_string(ctx, arg->v_string, exception);
        case GI_TYPE_TAG_FILENAME:
            return seed_value_from_filename(ctx, arg->v_string, exception);
        case GI_TYPE_TAG_GTYPE:
            return seed_value_from_int(ctx, arg->v_int, exception);
        case GI_TYPE_TAG_ARRAY: {

            GIArrayType array_type;
            GITypeInfo* array_type_info;
            JSValueRef ret;

            if (arg->v_pointer == NULL)
                return JSValueMakeNull(ctx);

            array_type = g_type_info_get_array_type(type_info);

            array_type_info = g_type_info_get_param_type(type_info, 0);

            if (array_type == GI_ARRAY_TYPE_PTR_ARRAY) {
                JSObjectRef ret_ptr_array;
                GPtrArray* ptr = arg->v_pointer;
                GArgument larg;
                int length = 0;
                int i;

                length = ptr->len;
                ret_ptr_array = JSObjectMakeArray(ctx, 0, NULL, exception);

                for (i = 0; i < length; ++i) {
                    JSValueRef ival;

                    larg.v_pointer = g_ptr_array_index(ptr, i);
                    ival = (JSValueRef) seed_value_from_gi_argument(
                      ctx, &larg, array_type_info, exception);
                    if (!ival)
                        ival = JSValueMakeNull(ctx);

                    JSObjectSetPropertyAtIndex(ctx, ret_ptr_array, i, ival,
                                               NULL);
                }

                g_base_info_unref((GIBaseInfo*) array_type_info);

                return ret_ptr_array;
            } else if (array_type == GI_ARRAY_TYPE_C) {

                if (g_type_info_is_zero_terminated(type_info)) {
                    GITypeInfo* param_info;
                    param_info = g_type_info_get_param_type(type_info, 0);
                    g_assert(param_info != NULL);

                    ret = seed_array_from_zero_terminated_c_array(
                      ctx, arg->v_pointer, param_info, exception);

                    g_base_info_unref((GIBaseInfo*) param_info);
                    g_base_info_unref((GIBaseInfo*) array_type_info);

                    return ret;
                }
            }

            // technically gir has arrays of bytes, eg.
            // <array length="2" zero-terminated="0" c:type="gchar**"><type
            // name="guint8"/>
            // example : g_file_get_contents..
            // we can treat this as a string.. - it's a bit flakey, we should
            // really use
            // Uint8Array - need to check the webkit API for this..

            if (!g_type_info_is_zero_terminated(type_info)
                && array_type == GI_ARRAY_TYPE_C
                && GI_TYPE_TAG_UINT8 == g_type_info_get_tag(array_type_info)
                && array_len > 0) {
                // got a stringy array..
                // we should check g_type_info_get_array_fixed_size
                // we are assuming that this is the array_len from the call..
                g_base_info_unref((GIBaseInfo*) array_type_info);

                ret = seed_value_from_binary_string(ctx, arg->v_pointer,
                                                    array_len, exception);
                // always free arg... what about when we do not own it...?
                // g_free(arg->v_pointer);

                return ret;
            }

            if (!g_type_info_is_zero_terminated(type_info)) {
                g_base_info_unref((GIBaseInfo*) array_type_info);
                break;
            }

            ret = seed_gi_make_jsarray(ctx, arg->v_pointer, array_type_info,
                                       exception);

            g_base_info_unref((GIBaseInfo*) array_type_info);

            return ret;
        }
        case GI_TYPE_TAG_INTERFACE: {
            GIBaseInfo* interface;
            GIInfoType interface_type;

            interface = g_type_info_get_interface(type_info);
            interface_type = g_base_info_get_type(interface);

            if (interface_type == GI_INFO_TYPE_OBJECT
                || interface_type == GI_INFO_TYPE_INTERFACE) {
                if (arg->v_pointer == 0) {
                    g_base_info_unref(interface);
                    return JSValueMakeNull(ctx);
                }
                g_base_info_unref(interface);
                return seed_value_from_object(ctx, arg->v_pointer, exception);
            } else if (interface_type == GI_INFO_TYPE_ENUM) {
                JSValueRef ret;
                GITypeTag etype = g_enum_info_get_storage_type(interface);

                // needs GITypeInfo - we are sending it  GIBaseInfo

                ret = seed_value_from_gi_argument_full(ctx, arg, type_info,
                                                       exception, 0, etype);
                g_base_info_unref(interface);
                return ret;
            } else if (interface_type == GI_INFO_TYPE_FLAGS) {

                g_base_info_unref(interface);
                return seed_value_from_long(ctx, arg->v_long, exception);
            } else if (interface_type == GI_INFO_TYPE_STRUCT) {

                // Trying to find out if this struct can be converted into
                // GValue
                // If it can be converted, just send the converted value back to
                // JS context instead
                // of the GValue itself.
                GType gtype = g_registered_type_info_get_g_type(
                  (GIRegisteredTypeInfo*) interface);
                if (G_TYPE_IS_INSTANTIATABLE(gtype)
                    || G_TYPE_IS_INTERFACE(gtype))
                    gtype = G_TYPE_FROM_INSTANCE(arg->v_pointer);

                SEED_NOTE(INVOCATION, "gtype of INTERFACE is %s",
                          g_type_name(gtype));
                if (g_type_is_a(gtype, G_TYPE_VALUE)) {
                    JSValueRef ret;
                    // We're using seed_value_from_gvalue_for_signal with NULL
                    // signal handler
                    // the current code will only handle basic types.
                    if ((ret = seed_value_from_gvalue_for_signal(
                           ctx, arg->v_pointer, exception, NULL, 0))) {
                        g_base_info_unref(interface);
                        return ret;
                    }
                }

                JSValueRef strukt;

                strukt = seed_make_struct(ctx, arg->v_pointer, interface);
                g_base_info_unref(interface);

                return strukt;
            } else if (interface_type == GI_INFO_TYPE_CALLBACK) {
                /* FIXME: return values of type callback are not handled yet. */
                g_base_info_unref(interface);
                return FALSE;
            }
            /* fall through - other types?? */
            g_base_info_unref(interface);
            return FALSE;
        }

        case GI_TYPE_TAG_GLIST: {
            GITypeInfo* list_type;
            JSObjectRef ret;
            GArgument larg;
            gint i = 0;
            GList* list = arg->v_pointer;

            ret = JSObjectMakeArray(ctx, 0, NULL, exception);
            list_type = g_type_info_get_param_type(type_info, 0);

            for (; list != NULL; list = list->next) {
                JSValueRef ival;

                larg.v_pointer = list->data;
                ival = (JSValueRef) seed_value_from_gi_argument(ctx, &larg,
                                                                list_type,
                                                                exception);
                if (!ival)
                    ival = JSValueMakeNull(ctx);
                JSObjectSetPropertyAtIndex(ctx, ret, i, ival, NULL);
                i++;
            }
            return ret;
        }
        case GI_TYPE_TAG_GSLIST: {
            GITypeInfo* list_type;
            JSObjectRef ret;
            JSValueRef ival;
            GArgument larg;
            guint i = 0;
            GSList* list = arg->v_pointer;

            ret = JSObjectMakeArray(ctx, 0, NULL, exception);
            list_type = g_type_info_get_param_type(type_info, 0);

            for (; list != NULL; list = list->next) {
                larg.v_pointer = list->data;
                ival = (JSValueRef) seed_value_from_gi_argument(ctx, &larg,
                                                                list_type,
                                                                exception);
                if (!ival)
                    ival = JSValueMakeNull(ctx);
                JSObjectSetPropertyAtIndex(ctx, ret, i, ival, NULL);
                i++;
            }
            return ret;
        }
        case GI_TYPE_TAG_GHASH: {
            GITypeInfo* key_type;
            GITypeTag key_type_tag;
            GITypeInfo* value_type;
            JSObjectRef ret;
            GHashTable* hash_table;
            GHashTableIter iter;
            gpointer key, value;

            key_type = g_type_info_get_param_type(type_info, 0);
            key_type_tag = g_type_info_get_tag(key_type);

            if ((key_type_tag != GI_TYPE_TAG_UTF8)
                && (key_type_tag != GI_TYPE_TAG_FILENAME)) {
                seed_make_exception(ctx, exception, "ArgumentError",
                                    "Unable to make object from hash table "
                                    "indexed with values of type %s",
                                    g_type_tag_to_string(key_type_tag));
                return JSValueMakeNull(ctx);
            }

            hash_table = arg->v_pointer;
            if (!hash_table) {
                seed_make_exception(
                  ctx, exception, "ArgumentError",
                  "Unable to make hash table indexed with values of type %s",
                  g_type_tag_to_string(key_type_tag));
                return JSValueMakeNull(ctx);
            }

            value_type = g_type_info_get_param_type(type_info, 1);

            ret = JSObjectMake(ctx, NULL, NULL);

            g_hash_table_iter_init(&iter, hash_table);

            while (g_hash_table_iter_next(&iter, &key, &value)) {
                JSStringRef js_key;
                GArgument value_arg;
                JSValueRef value_jsval;

                js_key = JSStringCreateWithUTF8CString((const char*) key);

                value_arg.v_string = value;
                value_jsval
                  = seed_value_from_gi_argument_full(ctx, &value_arg,
                                                     value_type, exception, 0,
                                                     key_type_tag);

                JSObjectSetProperty(ctx, ret, js_key, value_jsval, 0, NULL);
            }

            return ret;
        }
        case GI_TYPE_TAG_ERROR: {
            JSValueRef ret;
            seed_make_exception_from_gerror(ctx, &ret,
                                            (GError*) arg->v_pointer);
            return ret;
        }

        default:
            return FALSE;
    }
    return 0;
}

JSValueRef
seed_value_from_gvalue_for_signal(JSContextRef ctx,
                                  GValue* gval,
                                  JSValueRef* exception,
                                  GSignalQuery* signal_query,
                                  gint arg_n)
{
    if (!G_IS_VALUE(gval)) {
        return false;
    }
    switch (G_VALUE_TYPE(gval)) {
        case G_TYPE_BOOLEAN:
            return seed_value_from_boolean(ctx, g_value_get_boolean(gval),
                                           exception);
        case G_TYPE_CHAR:
            return seed_value_from_char(ctx, g_value_get_schar(gval),
                                        exception);
        case G_TYPE_UCHAR:
            return seed_value_from_uchar(ctx, g_value_get_uchar(gval),
                                         exception);
        case G_TYPE_INT:
            return seed_value_from_int(ctx, g_value_get_int(gval), exception);
        case G_TYPE_UINT:
            return seed_value_from_uint(ctx, g_value_get_uint(gval), exception);
        case G_TYPE_LONG:
            return seed_value_from_long(ctx, g_value_get_long(gval), exception);
        case G_TYPE_ULONG:
            return seed_value_from_ulong(ctx, g_value_get_ulong(gval),
                                         exception);
        case G_TYPE_INT64:
            return seed_value_from_int64(ctx, g_value_get_int64(gval),
                                         exception);
        case G_TYPE_UINT64:
            return seed_value_from_uint64(ctx, g_value_get_uint64(gval),
                                          exception);
        case G_TYPE_FLOAT:
            return seed_value_from_float(ctx, g_value_get_float(gval),
                                         exception);
        case G_TYPE_DOUBLE:
            return seed_value_from_double(ctx, g_value_get_double(gval),
                                          exception);
        case G_TYPE_STRING:
            return seed_value_from_string(ctx,
                                          (gchar*) g_value_get_string(gval),
                                          exception);
        case G_TYPE_POINTER:
            if (signal_query) {
                JSValueRef res;
                GArgument arg;
                GIBaseInfo* obj;
                GISignalInfo* signal_info;
                GIArgInfo* arg_info;
                GITypeInfo type_info;

                obj = g_irepository_find_by_gtype(NULL, signal_query->itype);
                if (!obj)
                    return NULL;

                signal_info
                  = g_object_info_find_signal((GIObjectInfo*) obj,
                                              signal_query->signal_name);

                if (!signal_info) {
                    g_base_info_unref((GIBaseInfo*) obj);
                    return NULL;
                }

                arg_info = g_callable_info_get_arg(signal_info, arg_n - 1);
                g_arg_info_load_type(arg_info, &type_info);

                arg.v_pointer = g_value_get_pointer(gval);

                res = seed_value_from_gi_argument(ctx, &arg, &type_info,
                                                  exception);

                g_base_info_unref((GIBaseInfo*) arg_info);
                g_base_info_unref((GIBaseInfo*) signal_info);
                g_base_info_unref((GIBaseInfo*) obj);

                return res;
            } else {
                return seed_make_pointer(ctx, g_value_get_pointer(gval));
            }
        case G_TYPE_PARAM:
            // Might need to dup and make a boxed.
            return seed_make_pointer(ctx, g_value_get_param(gval));
    }

    if (g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_STRV)) {
        return seed_value_from_strv(ctx, (GStrv*) g_value_get_boxed(gval),
                                    exception);
    }

    if (g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_ENUM)
        || g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_FLAGS))
        return seed_value_from_long(ctx, gval->data[0].v_long, exception);
    else if (g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_ENUM))
        return seed_value_from_long(ctx, gval->data[0].v_long, exception);
    else if (g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_OBJECT)) {
        GObject* obj = g_value_get_object(gval);
        return seed_value_from_object(ctx, obj, exception);
    } else {
        GIBaseInfo* info;
        GIInfoType type;

        info = g_irepository_find_by_gtype(0, G_VALUE_TYPE(gval));
        if (!info)
            return FALSE;
        type = g_base_info_get_type(info);

        if (type == GI_INFO_TYPE_UNION) {
            return seed_make_union(ctx, g_value_peek_pointer(gval), info);
        } else if (type == GI_INFO_TYPE_STRUCT) {
            return seed_make_struct(ctx, g_value_peek_pointer(gval), info);
        } else if (type == GI_INFO_TYPE_BOXED) {
            return seed_make_boxed(ctx, g_value_dup_boxed(gval), info);
        }
    }

    return NULL;
}

JSValueRef
seed_value_from_gvalue(JSContextRef ctx, GValue* gval, JSValueRef* exception)
{
    return seed_value_from_gvalue_for_signal(ctx, gval, exception, NULL, 0);
}

gboolean
seed_value_to_gvalue(JSContextRef ctx,
                     JSValueRef val,
                     GType type,
                     GValue* ret,
                     JSValueRef* exception)
{
    if (G_IS_VALUE(ret))
        g_value_unset(ret);

    if (type == G_TYPE_STRV) {
        gchar** result;
        JSValueRef jslen;
        guint length, i;

        if (JSValueIsNull(ctx, val) || !JSValueIsObject(ctx, val))
            return FALSE;

        jslen = seed_object_get_property(ctx, (JSObjectRef) val, "length");
        length = seed_value_to_uint(ctx, jslen, exception);

        result = g_new0(gchar*, length + 1);

        for (i = 0; i < length; i++) {
            result[i] = seed_value_to_string(
              ctx,
              JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) val, i, exception),
              exception);
        }
        result[i] = 0;

        g_value_init(ret, G_TYPE_STRV);
        g_value_take_boxed(ret, result);

        return TRUE;
    } else if (g_type_is_a(type, G_TYPE_ENUM) && JSValueIsNumber(ctx, val)) {
        g_value_init(ret, type);
        g_value_set_enum(ret, seed_value_to_long(ctx, val, exception));
        return TRUE;
    } else if (g_type_is_a(type, G_TYPE_FLAGS) && JSValueIsNumber(ctx, val)) {
        g_value_init(ret, type);
        g_value_set_flags(ret, seed_value_to_long(ctx, val, exception));
        return TRUE;
    } else if (g_type_is_a(type, G_TYPE_OBJECT)
               && (JSValueIsNull(ctx, val)
                   || seed_value_is_gobject(ctx, val))) {
        GObject* o = seed_value_to_object(ctx, val, exception);

        if (o == NULL || g_type_is_a(G_OBJECT_TYPE(o), type)) {
            g_value_init(ret, type);
            g_value_set_object(ret, o);

            return TRUE;
        }
    }
    /* Boxed handling is broken. Will be fixed in struct overhall. */
    else if (g_type_is_a(type, G_TYPE_BOXED)) {
        gpointer p = seed_pointer_get_pointer(ctx, val);
        if (p) {
            g_value_init(ret, type);
            g_value_set_boxed(ret, p);
            return TRUE;
        } else {
            if (JSValueIsObject(ctx, val)) {
                GIBaseInfo* info = g_irepository_find_by_gtype(0, type);
                JSObjectRef new_struct;
                if (!info)
                    return FALSE;

                new_struct = seed_construct_struct_type_with_parameters(
                  ctx, info, (JSObjectRef) val, exception);
                p = seed_pointer_get_pointer(ctx, new_struct);
                if (p) {
                    g_value_init(ret, type);
                    g_value_set_boxed(ret, p);
                    g_base_info_unref(info);
                    return TRUE;
                }
                g_base_info_unref(info);
            }
        }
    }

    switch (type) {
        case G_TYPE_BOOLEAN: {
            g_value_init(ret, G_TYPE_BOOLEAN);
            g_value_set_boolean(ret,
                                seed_value_to_boolean(ctx, val, exception));
            return TRUE;
        }
        case G_TYPE_INT:
        case G_TYPE_UINT: {
            g_value_init(ret, type);
            if (type == G_TYPE_INT)
                g_value_set_int(ret, seed_value_to_int(ctx, val, exception));
            else
                g_value_set_uint(ret, seed_value_to_uint(ctx, val, exception));
            return TRUE;
        }
        case G_TYPE_CHAR: {
            g_value_init(ret, G_TYPE_CHAR);
            g_value_set_schar(ret, seed_value_to_char(ctx, val, exception));
            return TRUE;
        }
        case G_TYPE_UCHAR: {
            g_value_init(ret, G_TYPE_UCHAR);
            g_value_set_uchar(ret, seed_value_to_uchar(ctx, val, exception));
            return TRUE;
        }
        case G_TYPE_LONG:
        case G_TYPE_ULONG:
        case G_TYPE_INT64:
        case G_TYPE_UINT64:
        case G_TYPE_FLOAT:
        case G_TYPE_DOUBLE: {
            switch (type) {
                case G_TYPE_LONG:
                    g_value_init(ret, G_TYPE_LONG);
                    g_value_set_long(ret,
                                     seed_value_to_long(ctx, val, exception));
                    break;
                case G_TYPE_ULONG:
                    g_value_init(ret, G_TYPE_ULONG);
                    g_value_set_ulong(ret,
                                      seed_value_to_ulong(ctx, val, exception));
                    break;
                case G_TYPE_INT64:
                    g_value_init(ret, G_TYPE_INT64);
                    g_value_set_int64(ret,
                                      seed_value_to_int64(ctx, val, exception));
                    break;
                case G_TYPE_UINT64:
                    g_value_init(ret, G_TYPE_UINT64);
                    g_value_set_uint64(ret, seed_value_to_uint64(ctx, val,
                                                                 exception));
                    break;
                case G_TYPE_FLOAT:
                    g_value_init(ret, G_TYPE_FLOAT);
                    g_value_set_float(ret,
                                      seed_value_to_float(ctx, val, exception));
                    break;
                case G_TYPE_DOUBLE:
                    g_value_init(ret, G_TYPE_DOUBLE);
                    g_value_set_double(ret, seed_value_to_double(ctx, val,
                                                                 exception));
                    break;
            }
            return TRUE;
        }
        case G_TYPE_STRING: {
            gchar* cval = seed_value_to_string(ctx, val, exception);

            g_value_init(ret, G_TYPE_STRING);
            g_value_take_string(ret, cval);

            return TRUE;
        }
        default: {
            // TODO: FIXME: This whole undefined type area
            // needs some heaaavy improvement.

            if (type == 0 && seed_value_is_gobject(ctx, val)) {
                GObject* o = seed_value_to_object(ctx, val, exception);

                g_value_init(ret, G_OBJECT_TYPE(o));
                g_value_set_object(ret, o);
                return TRUE;
            }
            // Support [GObject.TYPE_INT, 3]
            // TODO: FIXME: Might crash.
            else if (type == 0 && JSValueIsObject(ctx, val)) {
                // TODO: FIXME: Better array test like the cool one on reddit.
                guint length = seed_value_to_int(
                  ctx,
                  seed_object_get_property(ctx, (JSObjectRef) val, "length"),
                  exception);

                if (length) {
                    type = seed_value_to_int(
                      ctx, JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) val, 0,
                                                      exception),
                      exception);
                    val = JSObjectGetPropertyAtIndex(ctx, (JSObjectRef) val, 1,
                                                     exception);
                    if (type) // Prevents recursion.
                    {
                        return seed_value_to_gvalue(ctx, val, type, ret,
                                                    exception);
                    }
                    // TODO: FIXME: Handle better?
                    else
                        g_assert_not_reached();
                }
            }
            switch (JSValueGetType(ctx, val)) {
                case kJSTypeBoolean: {
                    g_value_init(ret, G_TYPE_BOOLEAN);
                    g_value_set_boolean(ret, seed_value_to_boolean(ctx, val,
                                                                   exception));
                    return TRUE;
                }
                case kJSTypeNumber: {
                    g_value_init(ret, G_TYPE_DOUBLE);
                    g_value_set_double(ret, seed_value_to_double(ctx, val,
                                                                 exception));
                    return TRUE;
                }
                case kJSTypeString: {
                    gchar* cv = seed_value_to_string(ctx, val, exception);

                    g_value_init(ret, G_TYPE_STRING);
                    g_value_take_string(ret, cv);
                    return TRUE;
                }
                default:
                    break;
            }
            break;
        }
    }

    return FALSE;
}

/**
 * seed_object_get_property
 * @ctx: A #SeedContext
 * @object: A #SeedObject
 * @name: The property to get, should be a valid JavaScript identifier
 *
 * Returns: The value of the property or the undefined value
 */
JSValueRef
seed_object_get_property(JSContextRef ctx, JSObjectRef val, const gchar* name)
{

    JSStringRef jname = JSStringCreateWithUTF8CString(name);
    JSValueRef ret = JSObjectGetProperty(ctx, (JSObjectRef) val, jname, NULL);

    JSStringRelease(jname);

    return ret;
}

/**
 * seed_object_set_property
 * @ctx: A #SeedContext
 * @object: A #SeedObject
 * @name: The property to set, should be a valid JavaScript identifier
 * @value: The value to set the property to.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 */
gboolean
seed_object_set_property(JSContextRef ctx,
                         JSObjectRef object,
                         const gchar* name,
                         JSValueRef value)
{
    JSStringRef jname = JSStringCreateWithUTF8CString(name);
    JSValueRef exception = NULL;

    if (value) {
        JSObjectSetProperty(ctx, (JSObjectRef) object, jname, value, 0,
                            &exception);
    }

    JSStringRelease(jname);

    return TRUE;
}

/* TODO: Make some macros or something for making exceptions, code is littered
   with annoyingness right now */

/**
 * seed_value_to_boolean:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gboolean. Keep in mind that this will
 * not convert a JavaScript number type, only a boolean.
 *
 * Return value: The #gboolean represented by @val.
 *
 */
gboolean
seed_value_to_boolean(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsBoolean(ctx, val) && !JSValueIsNumber(ctx, val)) {
        if (!JSValueIsNull(ctx, val)) {
            seed_make_exception(eng->context, exception, "ConversionError",
                                "Can not convert Javascript value to boolean");
            return FALSE;
        }

        return FALSE;
    }

    return JSValueToBoolean(ctx, val);
}

/**
 * seed_value_from_boolean:
 * @ctx: A #SeedContext.
 * @val: The #gboolean to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gboolean into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_boolean(JSContextRef ctx, gboolean val, JSValueRef* exception)
{
    return JSValueMakeBoolean(ctx, val);
}

/**
 * seed_value_to_uint:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #guint.
 *
 * Return value: The #guint represented by @val.
 *
 */
guint
seed_value_to_uint(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val) && !JSValueIsBoolean(ctx, val)) {
        if (!JSValueIsNull(ctx, val)) {
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " boolean");
        }
        return 0;
    }

    return (guint) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_uint:
 * @ctx: A #SeedContext.
 * @val: The #guint to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #guint into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_uint(JSContextRef ctx, guint val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_int:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gint.
 *
 * Return value: The #gint represented by @val.
 *
 */
gint
seed_value_to_int(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val) && !JSValueIsBoolean(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " int");
        return 0;
    }

    return (gint) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_int:
 * @ctx: A #SeedContext.
 * @val: The #gint to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gint into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_int(JSContextRef ctx, gint val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_char:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gchar.
 *
 * Return value: The #gchar represented by @val.
 *
 */
gchar
seed_value_to_char(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    gint cv;

    if (!JSValueIsNumber(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " gchar");
        return 0;
    }

    cv = JSValueToNumber(ctx, val, NULL);

    if (cv < G_MININT8 || cv > G_MAXINT8) {
        seed_make_exception(ctx, exception, "ConversionError",
                            "Javascript number out of range of gchar");
        return 0;
    }

    return (gchar) cv;
}

/**
 * seed_value_from_char:
 * @ctx: A #SeedContext.
 * @val: The #gchar to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gchar into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_char(JSContextRef ctx, gchar val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_uchar:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #guchar.
 *
 * Return value: The #guchar represented by @val.
 *
 */
guchar
seed_value_to_uchar(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    guint cv;

    if (!JSValueIsNumber(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " guchar");
        return 0;
    }

    cv = JSValueToNumber(ctx, val, NULL);

    if (cv > G_MAXUINT8) {
        seed_make_exception(ctx, exception, "ConversionError",
                            "Javascript number out of range of guchar");
        return 0;
    }

    return (guchar) cv;
}

/**
 * seed_value_from_uchar:
 * @ctx: A #SeedContext.
 * @val: The #guchar to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #guchar into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_uchar(JSContextRef ctx, guchar val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_short:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gshort.
 *
 * Return value: The #gshort represented by @val.
 *
 */
gshort
seed_value_to_short(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val) && !JSValueIsBoolean(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " short");
        return 0;
    }

    return (gshort) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_short:
 * @ctx: A #SeedContext.
 * @val: The #gshort to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gshort into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_short(JSContextRef ctx, gshort val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_ushort:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gushort.
 *
 * Return value: The #gushort represented by @val.
 *
 */
gushort
seed_value_to_ushort(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val) && !JSValueIsBoolean(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " ushort");
        return 0;
    }

    return (gushort) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_ushort:
 * @ctx: A #SeedContext.
 * @val: The #gushort to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gushort into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_ushort(JSContextRef ctx, gushort val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_long:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #glong.
 *
 * Return value: The #glong represented by @val.
 *
 */
glong
seed_value_to_long(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val) && !JSValueIsBoolean(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " long");
        return 0;
    }

    return (glong) JSValueToNumber(ctx, val, NULL);
}

/**
* seed_value_from_long:
* @ctx: A #SeedContext.
* @val: The #glong to represent.
* @exception: A reference to a #SeedValue in which to store any exceptions.
*             Pass %NULL to ignore exceptions.
*
* Converts the given #glong into a #SeedValue.
*
* Return value: A #SeedValue which represents @val.
*
*/
JSValueRef
seed_value_from_long(JSContextRef ctx, glong val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_ulong:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gulong.
 *
 * Return value: The #gulong represented by @val.
 *
 */
gulong
seed_value_to_ulong(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " ulong");

        return 0;
    }

    return (gulong) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_ulong:
 * @ctx: A #SeedContext.
 * @val: The #gulong to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gulong into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_ulong(JSContextRef ctx, gulong val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_int64:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gint64.
 *
 * Return value: The #gint64 represented by @val.
 *
 */
gint64
seed_value_to_int64(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val) && !JSValueIsBoolean(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " gint64");

        return 0;
    }

    return (gint64) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_int64:
 * @ctx: A #SeedContext.
 * @val: The #gint64 to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gint64 into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_int64(JSContextRef ctx, gint64 val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_uint64:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #guint64.
 *
 * Return value: The #guint64 represented by @val.
 *
 */
guint64
seed_value_to_uint64(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val) && !JSValueIsBoolean(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " guint64");

        return 0;
    }

    return (guint64) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_uint64:
 * @ctx: A #SeedContext.
 * @val: The #guint64 to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #guint64 into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_uint64(JSContextRef ctx, guint64 val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_float:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gfloat.
 *
 * Return value: The #gfloat represented by @val.
 *
 */
gfloat
seed_value_to_float(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " gfloat");
        return 0;
    }

    return (gfloat) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_float:
 * @ctx: A #SeedContext.
 * @val: The #gfloat to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gfloat into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_float(JSContextRef ctx, gfloat val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_double:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gdouble.
 *
 * Return value: The #gdouble represented by @val.
 *
 */
gdouble
seed_value_to_double(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " double");
        return 0;
    }

    return (gdouble) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_double:
 * @ctx: A #SeedContext.
 * @val: The #gdouble to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gdouble into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_double(JSContextRef ctx, gdouble val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_size:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gsize.
 *
 * Return value: The #gsize represented by @val.
 *
 */
gsize
seed_value_to_size(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val) && !JSValueIsBoolean(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " gsize");
        return 0;
    }

    return (gsize) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_size:
 * @ctx: A #SeedContext.
 * @val: The #gsize to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gsize into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_size(JSContextRef ctx, gsize val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_ssize:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gssize.
 *
 * Return value: The #gssize represented by @val.
 *
 */
gssize
seed_value_to_ssize(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    if (!JSValueIsNumber(ctx, val) && !JSValueIsBoolean(ctx, val)) {
        if (!JSValueIsNull(ctx, val))
            seed_make_exception(ctx, exception, "ConversionError",
                                "Can not convert Javascript value to"
                                " gssize");
        return 0;
    }

    return (gssize) JSValueToNumber(ctx, val, NULL);
}

/**
 * seed_value_from_ssize:
 * @ctx: A #SeedContext.
 * @val: The #gssize to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gssize into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_ssize(JSContextRef ctx, gssize val, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, (gdouble) val);
}

/**
 * seed_value_to_string:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gchar* string. Keep in mind that it's
 * up to the caller to free the string.
 *
 * If the #SeedValue represents JavaScript's undefined value, this returns
 * "[undefined]"; if it represents JavaScript's null value, this returns
 * "[null]".
 *
 * If the #SeedValue is a number or a boolean, it is printed as a double, with
 * the printf format string "%.15g".
 *
 * If the #SeedValue is an object, the string returned is that obtained by
 * calling .toString() on said object.
 *
 * Return value: The #gchar* represented by @val.
 *
 */
gchar*
seed_value_to_string(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    JSStringRef jsstr = NULL;
    JSValueRef func;
    gchar* buf = NULL;
    gint length;

    if (val == NULL)
        return NULL;
    else if (JSValueIsUndefined(ctx, val)) {
        buf = g_strdup("[undefined]");
    } else if (JSValueIsNull(ctx, val)) {
        buf = g_strdup("[null]");
    } else if (JSValueIsBoolean(ctx, val) || JSValueIsNumber(ctx, val)) {
        buf = g_strdup_printf("%.15g", JSValueToNumber(ctx, val, NULL));
    } else {
        if (!JSValueIsString(ctx, val)) // In this case,
        // it's an object
        {
            func = seed_object_get_property(ctx, (JSObjectRef) val, "toString");

            if (!JSValueIsNull(ctx, func) && JSValueIsObject(ctx, func)
                && JSObjectIsFunction(ctx, (JSObjectRef) func))
                // str = ... we dump the return value!?!
                JSObjectCallAsFunction(ctx, (JSObjectRef) func,
                                       (JSObjectRef) val, 0, NULL, NULL);
        }

        jsstr = JSValueToStringCopy(ctx, val, NULL);
        length = JSStringGetMaximumUTF8CStringSize(jsstr);
        if (length > 0) {
            buf = g_malloc(length * sizeof(gchar));
            JSStringGetUTF8CString(jsstr, buf, length);
        }
        if (jsstr)
            JSStringRelease(jsstr);
    }

    return buf;
}

/**
 * seed_value_from_string:
 * @ctx: A #SeedContext.
 * @val: The #gchar* to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gchar* string into a #SeedValue.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_string(JSContextRef ctx,
                       const gchar* val,
                       JSValueRef* exception)
{
    if (val == NULL)
        return JSValueMakeNull(ctx);
    else {
        JSStringRef jsstr = JSStringCreateWithUTF8CString(val);
        JSValueRef valstr = JSValueMakeString(ctx, jsstr);
        JSStringRelease(jsstr);

        return valstr;
    }
}

/**
 * seed_value_from_binary_string:
 * @ctx: A #SeedContext.
 * @bytes: A string of bytes to represent as a string.
 * @n_bytes: The number of bytes from @bytes to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts a string representation of the given binary string
 * into a #SeedValue.
 *
 * FIXME - should use BinaryArray really
 *
 * Return value: A #SeedValue which represents @bytes as a string.
 *
 */
JSValueRef
seed_value_from_binary_string(JSContextRef ctx,
                              const gchar* bytes,
                              gint n_bytes,
                              JSValueRef* exception)
{
    JSStringRef jsstr;
    JSValueRef valstr;
    JSChar* jchar;
    gint i;

    SEED_NOTE(INVOCATION, "Creating binary string of length %d ", n_bytes);

    if (bytes == NULL) {
        return JSValueMakeNull(ctx);
    }

    jchar = g_new0(JSChar, n_bytes);
    for (i = 0; i < n_bytes; i++) {
        jchar[i] = bytes[i];
    }
    // this may leak...

    jsstr = JSStringCreateWithCharacters((const JSChar*) jchar, n_bytes);
    valstr = JSValueMakeString(ctx, jsstr);
    g_free(jchar);
    JSStringRelease(jsstr);

    return valstr;
}

/**
 * seed_value_to_filename:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to convert.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #SeedValue into a #gchar*, properly converting to the
 * character set used for filenames on the local machine.
 *
 * Return value: The #gchar* represented by @val, or %NULL if an exception
 *               is raised during the conversion.
 *
 */
gchar*
seed_value_to_filename(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    GError* e = NULL;
    gchar* utf8 = seed_value_to_string(ctx, val, exception);
    gchar* filename;

    filename = g_filename_from_utf8(utf8, -1, NULL, NULL, &e);
    g_free(utf8);
    if (e) {
        seed_make_exception_from_gerror(ctx, exception, e);
        g_error_free(e);
        return NULL;
    }

    return filename;
}

/**
 * seed_value_from_filename:
 * @ctx: A #SeedContext.
 * @val: The #gchar* filename to represent.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Converts the given #gchar* filename into a #SeedValue, respecting the
 * character set used for filenames on the local machine.
 *
 * Return value: A #SeedValue which represents @val.
 *
 */
JSValueRef
seed_value_from_filename(JSContextRef ctx,
                         const gchar* val,
                         JSValueRef* exception)
{
    GError* e = NULL;
    gchar* utf8;

    if (val == NULL)
        return JSValueMakeNull(ctx);
    else {
        utf8 = g_filename_to_utf8(val, -1, NULL, NULL, &e);

        if (e) {
            seed_make_exception_from_gerror(ctx, exception, e);
            g_error_free(e);
            return JSValueMakeNull(ctx);
        }

        JSValueRef valstr = seed_value_from_string(ctx, utf8, exception);

        g_free(utf8);

        return valstr;
    }
}

/**
 * seed_value_to_object:
 * @ctx: A #SeedContext.
 * @val: The #SeedValue to unwrap.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Given a #SeedValue which is wrapping a #GObject, retrieve the wrapped
 * #GObject.
 *
 * Return value: The #GObject wrapped within @val, or %NULL if an exception
 *               is raised during the conversion.
 *
 */
GObject*
seed_value_to_object(JSContextRef ctx, JSValueRef val, JSValueRef* exception)
{
    GObject* gobject;

    /*
     * Worth investigating if this is the best way to handle null. Some of
     * the existing code depends on null Objects not throwing an exception
     * however, needs testing at higher level if value can be null
     * (through GI)
     */

    if (JSValueIsNull(ctx, val))
        return NULL;
    if (!seed_value_is_gobject(ctx, val)) {
        seed_make_exception(ctx, exception, "ConversionError",
                            "Attempt to convert from"
                            " non GObject to GObject");
        return NULL;
    }

    gobject = (GObject*) JSObjectGetPrivate((JSObjectRef) val);
    g_assert(G_IS_OBJECT(gobject));

    return gobject;
}

/**
 * seed_value_from_object:
 * @ctx: A #SeedContext.
 * @val: The #GObject to wrap.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Wraps @val in a #SeedValue.
 *
 * Return value: A #SeedValue which wraps @val.
 *
 */
JSValueRef
seed_value_from_object(JSContextRef ctx, GObject* val, JSValueRef* exception)
{
    if (val == NULL)
        return JSValueMakeNull(ctx);
    else
        return seed_wrap_object(ctx, val);
}

gboolean
seed_validate_enum(GIEnumInfo* info, long val)
{
    gint n, i;
    GIValueInfo* value_info;
    gint
      value; // TODO: investigate glong/gint mismatch w/ g_value_info_get_value

    n = g_enum_info_get_n_values(info);
    for (i = 0; i < n; i++) {
        value_info = g_enum_info_get_value(info, i);
        value = g_value_info_get_value(value_info);

        g_base_info_unref((GIBaseInfo*) value_info);
        if (value == val)
            return TRUE;
    }

    return FALSE;
}

JSValueRef
seed_value_from_time_t(JSContextRef ctx, time_t time, JSValueRef* exception)
{
    JSValueRef args[1];

    args[0] = seed_value_from_double(ctx, ((gdouble) time) * 1000, exception);
    return JSObjectMakeDate(ctx, 1, args, exception);
}

time_t
seed_value_to_time_t(JSContextRef ctx, JSValueRef value, JSValueRef* exception)
{
    JSValueRef get_time_method;
    JSValueRef jstime;
    gdouble time;

    if (JSValueIsNumber(ctx, value)) {
        return (unsigned long) seed_value_to_long(ctx, value, exception);
    } else if (JSValueIsObject(ctx, value)) {
        get_time_method
          = seed_object_get_property(ctx, (JSObjectRef) value, "getTime");
        if (JSValueIsNull(ctx, get_time_method)
            || !JSValueIsObject(ctx, get_time_method)) {
            goto out;
        }
        jstime
          = JSObjectCallAsFunction(ctx, (JSObjectRef) get_time_method,
                                   (JSObjectRef) value, 0, NULL, exception);
        time = seed_value_to_double(ctx, jstime, exception);
        return (unsigned long) (time / 1000);
    }

out:
    seed_make_exception(ctx, exception, "TypeError",
                        "Unable to convert JavaScript value to time_t");
    return 0;
}

/**
 * seed_value_from_strv:
 * @ctx: A #SeedContext.
 * @val: The #GStrv to wrap.
 * @exception: A reference to a #SeedValue in which to store any exceptions.
 *             Pass %NULL to ignore exceptions.
 *
 * Wraps @val in a #SeedValue.
 *
 * Return value: A #SeedValue which wraps @val.
 *
 */
JSValueRef
seed_value_from_strv(JSContextRef ctx, GStrv* val, JSValueRef* exception)
{
    GArray* js_string_array = g_array_new(FALSE, FALSE, sizeof(gpointer));
    for (; *val != NULL; val++) {
        JSValueRef js_string
          = seed_value_from_string(ctx, (char*) *val, exception);
        js_string_array = g_array_append_val(js_string_array, js_string);
    }
    JSValueRef res
      = JSObjectMakeArray(ctx, js_string_array->len,
                          (JSValueRef*) js_string_array->data, exception);
    g_array_free(js_string_array, FALSE);

    return res;
}
