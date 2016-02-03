/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/* Copyright 2008 litl, LLC. All Rights Reserved. */

#ifndef __BIG_UTIL_DBUS_PROXY_H__
#define __BIG_UTIL_DBUS_PROXY_H__

#include <gio/gio.h>
#include <dbus/dbus.h>

G_BEGIN_DECLS

typedef struct _BigDBusProxy BigDBusProxy;
typedef struct _BigDBusProxyClass BigDBusProxyClass;

typedef void (*BigDBusProxyReplyFunc)(BigDBusProxy* proxy,
                                      DBusMessage* message,
                                      void* data);
typedef void (*BigDBusProxyJsonReplyFunc)(BigDBusProxy* proxy,
                                          DBusMessage* message,
                                          DBusMessageIter* return_value_iter,
                                          void* data);
typedef void (*BigDBusProxyErrorReplyFunc)(BigDBusProxy* proxy,
                                           const char* error_name,
                                           const char* error_message,
                                           void* data);

#define BIG_TYPE_DBUS_PROXY (big_dbus_proxy_get_type())
#define BIG_DBUS_PROXY(object)                                                 \
    (G_TYPE_CHECK_INSTANCE_CAST((object), BIG_TYPE_DBUS_PROXY, BigDBusProxy))
#define BIG_DBUS_PROXY_CLASS(klass)                                            \
    (G_TYPE_CHECK_CLASS_CAST((klass), BIG_TYPE_DBUS_PROXY, BigDBusProxyClass))
#define BIG_IS_DBUS_PROXY(object)                                              \
    (G_TYPE_CHECK_INSTANCE_TYPE((object), BIG_TYPE_DBUS_PROXY))
#define BIG_IS_DBUS_PROXY_CLASS(klass)                                         \
    (G_TYPE_CHECK_CLASS_TYPE((klass), BIG_TYPE_DBUS_PROXY))
#define BIG_DBUS_PROXY_GET_CLASS(obj)                                          \
    (G_TYPE_INSTANCE_GET_CLASS((obj), BIG_TYPE_DBUS_PROXY, BigDBusProxyClass))

GType big_dbus_proxy_get_type(void) G_GNUC_CONST;

BigDBusProxy* big_dbus_proxy_new(DBusConnection* connection,
                                 const char* bus_name,
                                 const char* object_path,
                                 const char* iface);
DBusConnection* big_dbus_proxy_get_connection(BigDBusProxy* proxy);
const char* big_dbus_proxy_get_bus_name(BigDBusProxy* proxy);
DBusMessage* big_dbus_proxy_new_method_call(BigDBusProxy* proxy,
                                            const char* method_name);
DBusMessage* big_dbus_proxy_new_json_call(BigDBusProxy* proxy,
                                          const char* method_name,
                                          DBusMessageIter* arg_iter,
                                          DBusMessageIter* dict_iter);
void big_dbus_proxy_send(BigDBusProxy* proxy,
                         DBusMessage* message,
                         BigDBusProxyReplyFunc reply_func,
                         BigDBusProxyErrorReplyFunc error_func,
                         void* data);

/* varargs are like:
 *
 *   key1, dbus_type_1, &value_1,
 *   key2, dbus_type_2, &value_2,
 *   NULL
 *
 * Basic types only (no arrays)
 */
void big_dbus_proxy_call_json_async(BigDBusProxy* proxy,
                                    const char* method_name,
                                    BigDBusProxyJsonReplyFunc reply_func,
                                    BigDBusProxyErrorReplyFunc error_func,
                                    void* data,
                                    const char* first_key,
                                    ...);

G_END_DECLS

#endif /* __BIG_UTIL_DBUS_PROXY_H__ */
