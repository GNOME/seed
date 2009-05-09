/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/* Copyright 2008 litl, LLC. All Rights Reserved. */

#include <config.h>

#include "dbus-proxy.h"
#include "dbus.h"
#include "log.h"
#include <dbus/dbus-glib-lowlevel.h>
#include <stdarg.h>

typedef enum {
    REPLY_CLOSURE_PLAIN,
    REPLY_CLOSURE_JSON
} ReplyClosureType;

typedef struct {
    BigDBusProxy *proxy;
    ReplyClosureType type;
    union {
        BigDBusProxyReplyFunc     plain;
        BigDBusProxyJsonReplyFunc json;
    } func;
    BigDBusProxyErrorReplyFunc error_func;
    void *data;
    /* this is a debug thing; we want to guarantee
     * we call exactly 1 time either the reply or error
     * callback.
     */
    guint reply_invoked : 1;
    guint error_invoked : 1;
} ReplyClosure;

static void     big_dbus_proxy_dispose      (GObject               *object);
static void     big_dbus_proxy_finalize     (GObject               *object);
static GObject* big_dbus_proxy_constructor  (GType                  type,
                                             guint                  n_construct_properties,
                                             GObjectConstructParam *construct_params);
static void     big_dbus_proxy_get_property (GObject               *object,
                                             guint                  prop_id,
                                             GValue                *value,
                                             GParamSpec            *pspec);
static void     big_dbus_proxy_set_property (GObject               *object,
                                             guint                  prop_id,
                                             const GValue          *value,
                                             GParamSpec            *pspec);

struct _BigDBusProxy {
    GObject parent;

    DBusConnection *connection;
    char *bus_name;
    char *object_path;
    char *iface;
};

struct _BigDBusProxyClass {
    GObjectClass parent;
};

G_DEFINE_TYPE(BigDBusProxy, big_dbus_proxy, G_TYPE_OBJECT);

#if 0
enum {
    LAST_SIGNAL
};

static int signals[LAST_SIGNAL];
#endif

enum {
    PROP_0,
    PROP_CONNECTION,
    PROP_BUS_NAME,
    PROP_OBJECT_PATH,
    PROP_INTERFACE
};

static void
big_dbus_proxy_init(BigDBusProxy *proxy)
{

}

static void
big_dbus_proxy_class_init(BigDBusProxyClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = big_dbus_proxy_dispose;
    object_class->finalize = big_dbus_proxy_finalize;

    object_class->constructor = big_dbus_proxy_constructor;
    object_class->get_property = big_dbus_proxy_get_property;
    object_class->set_property = big_dbus_proxy_set_property;

    g_object_class_install_property(object_class,
                                    PROP_CONNECTION,
                                    g_param_spec_boxed("connection",
                                                       "DBusConnection",
                                                       "Our connection to the bus",
                                                       DBUS_TYPE_CONNECTION,
                                                       G_PARAM_READWRITE));
    g_object_class_install_property(object_class,
                                    PROP_BUS_NAME,
                                    g_param_spec_string("bus-name",
                                                        "Bus Name",
                                                        "Name of app on the bus",
                                                        NULL,
                                                        G_PARAM_READWRITE));
    g_object_class_install_property(object_class,
                                    PROP_OBJECT_PATH,
                                    g_param_spec_string("object-path",
                                                        "Object Path",
                                                        "Object's dbus path",
                                                        NULL,
                                                        G_PARAM_READWRITE));

    g_object_class_install_property(object_class,
                                    PROP_INTERFACE,
                                    g_param_spec_string("interface",
                                                        "Interface",
                                                        "Interface to invoke methods on",
                                                        NULL,
                                                        G_PARAM_READWRITE));
}

static void
big_dbus_proxy_dispose(GObject *object)
{
    BigDBusProxy *proxy;

    proxy = BIG_DBUS_PROXY(object);

    if (proxy->connection) {
        dbus_connection_unref(proxy->connection);
        proxy->connection = NULL;
    }

    if (proxy->bus_name) {
        g_free(proxy->bus_name);
        proxy->bus_name = NULL;
    }

    if (proxy->object_path) {
        g_free(proxy->object_path);
        proxy->object_path = NULL;
    }

    if (proxy->iface) {
        g_free(proxy->iface);
        proxy->iface = NULL;
    }

    G_OBJECT_CLASS(big_dbus_proxy_parent_class)->dispose(object);
}

static void
big_dbus_proxy_finalize(GObject *object)
{

    G_OBJECT_CLASS(big_dbus_proxy_parent_class)->finalize(object);
}

static GObject*
big_dbus_proxy_constructor (GType                  type,
                            guint                  n_construct_properties,
                            GObjectConstructParam *construct_params)
{
    GObject *object;
    BigDBusProxy *proxy;

    object = (* G_OBJECT_CLASS (big_dbus_proxy_parent_class)->constructor) (type,
                                                                            n_construct_properties,
                                                                            construct_params);

    proxy = BIG_DBUS_PROXY(object);

    return object;
}

static void
big_dbus_proxy_get_property (GObject     *object,
                             guint        prop_id,
                             GValue      *value,
                             GParamSpec  *pspec)
{
    BigDBusProxy *proxy;

    proxy = BIG_DBUS_PROXY (object);

    switch (prop_id) {
    case PROP_CONNECTION:
        g_value_set_boxed(value, proxy->connection);
        break;
    case PROP_BUS_NAME:
        g_value_set_string(value, proxy->bus_name);
        break;
    case PROP_OBJECT_PATH:
        g_value_set_string(value, proxy->object_path);
        break;
    case PROP_INTERFACE:
        g_value_set_string(value, proxy->iface);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
big_dbus_proxy_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
    BigDBusProxy *proxy;

    proxy = BIG_DBUS_PROXY (object);

    switch (prop_id) {
    case PROP_CONNECTION:
        if (proxy->connection != NULL) {
            g_warning("Cannot change BigDBusProxy::connection after it's set");
            return;
        }
        proxy->connection = dbus_connection_ref(g_value_get_boxed(value));
        break;
    case PROP_BUS_NAME:
        if (proxy->bus_name != NULL) {
            g_warning("Cannot change BigDBusProxy::bus-name after it's set");
            return;
        }
        proxy->bus_name = g_value_dup_string(value);
        break;
    case PROP_OBJECT_PATH:
        if (proxy->object_path != NULL) {
            g_warning("Cannot change BigDBusProxy::object-path after it's set");
            return;
        }
        proxy->object_path = g_value_dup_string(value);
        break;
    case PROP_INTERFACE:
        if (proxy->iface != NULL) {
            g_warning("Cannot change BigDBusProxy::interface after it's set");
            return;
        }
        proxy->iface = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

/* bus_name can be NULL if not going through a bus, and
 * iface is allowed to be NULL but likely should not be.
 */
BigDBusProxy*
big_dbus_proxy_new(DBusConnection *connection,
                   const char     *bus_name,
                   const char     *object_path,
                   const char     *iface)
{
    BigDBusProxy *proxy;

    g_return_val_if_fail(connection != NULL, NULL);
    g_return_val_if_fail(object_path != NULL, NULL);

    proxy = g_object_new(BIG_TYPE_DBUS_PROXY,
                         "connection", connection,
                         "bus-name", bus_name,
                         "object-path", object_path,
                         "interface", iface,
                         NULL);

    return proxy;
}

DBusConnection*
big_dbus_proxy_get_connection(BigDBusProxy *proxy)
{
    return proxy->connection;
}

const char*
big_dbus_proxy_get_bus_name(BigDBusProxy *proxy)
{
    return proxy->bus_name;
}

DBusMessage*
big_dbus_proxy_new_method_call(BigDBusProxy *proxy,
                               const char   *method_name)
{
    DBusMessage *message;

    message = dbus_message_new_method_call(proxy->bus_name,
                                           proxy->object_path,
                                           proxy->iface,
                                           method_name);
    if (message == NULL)
        g_error("no memory");

    /* We don't want methods to auto-start services...  if a service
     * needs starting or restarting, we want to do so explicitly so we
     * can do it in an orderly and predictable way.
     */
    dbus_message_set_auto_start(message, FALSE);

    return message;
}

DBusMessage*
big_dbus_proxy_new_json_call(BigDBusProxy    *proxy,
                             const char      *method_name,
                             DBusMessageIter *arg_iter,
                             DBusMessageIter *dict_iter)
{
    DBusMessage *message;

    message = big_dbus_proxy_new_method_call(proxy, method_name);

    dbus_message_iter_init_append(message, arg_iter);
    dbus_message_iter_open_container(arg_iter, DBUS_TYPE_ARRAY, "{sv}", dict_iter);

    return message;
}

static ReplyClosure*
reply_closure_new(BigDBusProxy              *proxy,
                  BigDBusProxyReplyFunc      plain_func,
                  BigDBusProxyJsonReplyFunc  json_func,
                  BigDBusProxyErrorReplyFunc error_func,
                  void                      *data)
{
    ReplyClosure *c;

    c = g_slice_new0(ReplyClosure);

    c->proxy = g_object_ref(proxy);

    g_assert(!(plain_func && json_func));

    if (plain_func != NULL) {
        c->type = REPLY_CLOSURE_PLAIN;
        c->func.plain = plain_func;
    } else {
        c->type = REPLY_CLOSURE_JSON;
        c->func.json = json_func;
    }

    c->error_func = error_func;
    c->data = data;

    return c;
}

static void
reply_closure_free(ReplyClosure *c)
{
    /* call exactly one of these */
    g_assert(!(c->error_invoked &&
               c->reply_invoked));

    if (!(c->error_invoked ||
          c->reply_invoked)) {
        c->error_invoked = TRUE;
        if (c->error_func) {
            (* c->error_func) (c->proxy, DBUS_ERROR_FAILED,
                               "Pending call was freed (due to dbus_shutdown() probably) before it was ever notified",
                               c->data);
        }
    }

    g_object_unref(c->proxy);
    g_slice_free(ReplyClosure, c);
}

static void
reply_closure_invoke_error(ReplyClosure *c,
                           DBusMessage  *reply)
{
    g_assert(dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_ERROR);

    g_assert(!c->reply_invoked);
    g_assert(!c->error_invoked);

    c->error_invoked = TRUE;

    if (c->error_func) {
        DBusError derror;

        dbus_error_init(&derror);

        dbus_set_error_from_message(&derror, reply);

        (* c->error_func) (c->proxy, derror.name,
                           derror.message,
                           c->data);

        dbus_error_free(&derror);
    }
}

static void
reply_closure_invoke(ReplyClosure *c,
                     DBusMessage  *reply)
{
    if (c->type == REPLY_CLOSURE_PLAIN) {
        if (dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN) {
            g_assert(!c->reply_invoked);
            g_assert(!c->error_invoked);

            c->reply_invoked = TRUE;

            if (c->func.plain != NULL) {
                (* c->func.plain) (c->proxy,
                                   reply,
                                   c->data);
            }
        } else if (dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_ERROR) {
            reply_closure_invoke_error(c, reply);
        } else {
            g_assert(!c->reply_invoked);
            g_assert(!c->error_invoked);

            c->error_invoked = TRUE;

            if (c->error_func) {
                (* c->error_func) (c->proxy, DBUS_ERROR_FAILED,
                                   "Got weird message type back as a reply",
                                   c->data);
            }
        }
    } else if (c->type == REPLY_CLOSURE_JSON) {
        if (dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN) {
            if (dbus_message_has_signature(reply, "a{sv}")) {
                g_assert(!c->reply_invoked);
                g_assert(!c->error_invoked);

                c->reply_invoked = TRUE;

                if (c->func.json) {
                    DBusMessageIter arg_iter;
                    DBusMessageIter dict_iter;

                    dbus_message_iter_init(reply, &arg_iter);
                    dbus_message_iter_recurse(&arg_iter, &dict_iter);

                    (* c->func.json) (c->proxy,
                                      reply,
                                      &dict_iter,
                                      c->data);
                }
            } else {
                g_assert(!c->reply_invoked);
                g_assert(!c->error_invoked);

                c->error_invoked = TRUE;

                if (c->error_func) {
                    (* c->error_func) (c->proxy,
                                       DBUS_ERROR_FAILED,
                                       "Message we got back did not have the right signature",
                                       c->data);
                }
            }
        } else if (dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_ERROR) {
            reply_closure_invoke_error(c, reply);
        } else {
            g_assert(!c->reply_invoked);
            g_assert(!c->error_invoked);

            c->error_invoked = TRUE;

            if (c->error_func) {
                (* c->error_func) (c->proxy, DBUS_ERROR_FAILED,
                                   "Got weird message type back as a reply",
                                   c->data);
            }
        }
    } else {
        g_assert_not_reached();
    }
}


static gboolean
failed_to_send_idle(void *data)
{
    ReplyClosure *c;

    c = data;

    g_assert(!c->reply_invoked);
    g_assert(!c->error_invoked);

    c->error_invoked = TRUE;

    if (c->error_func) {
        (* c->error_func) (c->proxy,
                           DBUS_ERROR_NO_MEMORY,
                           "Unable to send method call",
                           c->data);
    }

    reply_closure_free(c);

    return FALSE;
}


static void
pending_call_notify(DBusPendingCall *pending,
                    void            *user_data)
{
    DBusMessage *reply;
    ReplyClosure *c;

    big_debug(BIG_DEBUG_UTIL_DBUS,
              "BigDBusProxy received reply to pending call");

    c = user_data;

    /* reply may be NULL if none received? I think it may never be if
     * we've already been notified, but be safe here.
     */
    reply = dbus_pending_call_steal_reply(pending);

    if (reply) {
        reply_closure_invoke(c, reply);

        dbus_message_unref(reply);
    } else {
        /* I think libdbus won't let this happen, but to be safe... */
        g_assert(!c->reply_invoked);
        g_assert(!c->error_invoked);

        c->error_invoked = TRUE;

        if (c->error_func) {
            (* c->error_func) (c->proxy,
                               DBUS_ERROR_TIMED_OUT,
                               "Did not receive a reply or error",
                               c->data);
        }
    }

    /* The closure should be freed along with the pending call */
}

static void
pending_call_free_data(void *data)
{
    ReplyClosure *c = data;
    reply_closure_free(c);
}

static void
big_dbus_proxy_send_internal(BigDBusProxy              *proxy,
                             DBusMessage               *message,
                             BigDBusProxyReplyFunc      plain_func,
                             BigDBusProxyJsonReplyFunc  json_func,
                             BigDBusProxyErrorReplyFunc error_func,
                             void                      *data)
{
    ReplyClosure *c;
    DBusPendingCall *pending;

    if (!(plain_func || json_func || error_func)) {
        /* Fire and forget! */

        big_debug(BIG_DEBUG_UTIL_DBUS,
                  "Firing and forgetting dbus proxy call");

        dbus_connection_send(proxy->connection, message, NULL);
        return;
    }

    big_debug(BIG_DEBUG_UTIL_DBUS,
              "Sending dbus proxy call %s",
              dbus_message_get_member(message));

    c = reply_closure_new(proxy, plain_func, json_func, error_func, data);
    pending = NULL;
    if (!dbus_connection_send_with_reply(proxy->connection, message, &pending, -1) ||
        pending == NULL) {

        big_debug(BIG_DEBUG_UTIL_DBUS,
                  "Failed to send call, will report error in idle handler");

        /* Send an error on return to main loop */
        g_idle_add(failed_to_send_idle, c);
        return;
    }

    dbus_pending_call_set_notify(pending, pending_call_notify, c,
                                 pending_call_free_data);

    dbus_pending_call_unref(pending); /* DBusConnection should still hold a ref until it's completed */
}

void
big_dbus_proxy_send(BigDBusProxy              *proxy,
                    DBusMessage               *message,
                    BigDBusProxyReplyFunc      reply_func,
                    BigDBusProxyErrorReplyFunc error_func,
                    void                      *data)
{
    big_dbus_proxy_send_internal(proxy, message, reply_func, NULL, error_func, data);
}

static void
append_entries_from_valist(DBusMessageIter *dict_iter,
                           const char      *first_key,
                           va_list          args)
{
    const char *key;
    int dbus_type;
    void *value_p;

    key = first_key;
    dbus_type = va_arg(args, int);
    value_p = va_arg(args, void*);

    big_dbus_append_json_entry(dict_iter, key, dbus_type, value_p);

    key = va_arg(args, const char*);
    while (key != NULL) {
        dbus_type = va_arg(args, int);
        value_p = va_arg(args, void*);

        big_dbus_append_json_entry(dict_iter, key, dbus_type, value_p);

        key = va_arg(args, const char*);
    }
}

void
big_dbus_proxy_call_json_async (BigDBusProxy              *proxy,
                                const char                *method_name,
                                BigDBusProxyJsonReplyFunc  reply_func,
                                BigDBusProxyErrorReplyFunc error_func,
                                void                      *data,
                                const char                *first_key,
                                ...)
{
    DBusMessageIter arg_iter, dict_iter;
    DBusMessage *message;
    va_list args;

    message = big_dbus_proxy_new_json_call(proxy, method_name, &arg_iter, &dict_iter);

    if (first_key != NULL) {
        va_start(args, first_key);
        append_entries_from_valist(&dict_iter, first_key, args);
        va_end(args);
    }

    dbus_message_iter_close_container(&arg_iter, &dict_iter);

    big_dbus_proxy_send_internal(proxy, message, NULL, reply_func, error_func, data);

    dbus_message_unref(message);
}
