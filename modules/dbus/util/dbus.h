/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/* Copyright 2008 litl, LLC. All Rights Reserved. */

#ifndef __BIG_UTIL_DBUS_H__
#define __BIG_UTIL_DBUS_H__

#include <glib-object.h>
#include <dbus/dbus.h>

G_BEGIN_DECLS

/* Convenience macro */

#define BIG_DBUS_NAME_FROM_TYPE(type)                                          \
    ((type) == DBUS_BUS_SESSION ? "session" : "system")

/* Error names */
#define BIG_DBUS_ERROR_STREAM_RECEIVER_CLOSED                                  \
    "com.litl.Error.Stream.ReceiverClosed"

/*
 * Monitor whether we are connected / not-connected to the bus
 */

typedef void (*BigDBusConnectionOpenedFunc)(DBusConnection* connection,
                                            void* data);
typedef void (*BigDBusConnectionClosedFunc)(DBusConnection* connection,
                                            void* data);

typedef struct
{
    DBusBusType which_bus;
    BigDBusConnectionOpenedFunc opened;
    BigDBusConnectionClosedFunc closed;
} BigDBusConnectFuncs;

void big_dbus_add_connect_funcs(const BigDBusConnectFuncs* funcs, void* data);
void big_dbus_remove_connect_funcs(const BigDBusConnectFuncs* funcs,
                                   void* data);
void big_dbus_add_connect_funcs_sync_notify(const BigDBusConnectFuncs* funcs,
                                            void* data);

void big_dbus_add_bus_weakref(DBusBusType bus_type,
                              DBusConnection** connection_p);
void big_dbus_remove_bus_weakref(DBusBusType bus_type,
                                 DBusConnection** connection_p);

void big_dbus_try_connecting_now(DBusBusType which_bus);

/*
 * Own a bus name
 *
 */

typedef enum {
    BIG_DBUS_NAME_SINGLE_INSTANCE,
    BIG_DBUS_NAME_MANY_INSTANCES
} BigDBusNameType;

typedef void (*BigDBusNameAcquiredFunc)(DBusConnection* connection,
                                        const char* name,
                                        void* data);
typedef void (*BigDBusNameLostFunc)(DBusConnection* connection,
                                    const char* name,
                                    void* data);

typedef struct
{
    const char* name;
    BigDBusNameType type;
    BigDBusNameAcquiredFunc acquired;
    BigDBusNameLostFunc lost;
} BigDBusNameOwnerFuncs;

guint big_dbus_acquire_name(DBusBusType bus_type,
                            const BigDBusNameOwnerFuncs* funcs,
                            void* data);
void big_dbus_release_name(DBusBusType bus_type,
                           const BigDBusNameOwnerFuncs* funcs,
                           void* data);
void big_dbus_release_name_by_id(DBusBusType bus_type, guint id);

/*
 * Keep track of someone else's bus name
 *
 */

typedef enum { BIG_DBUS_NAME_START_IF_NOT_FOUND = 0x1 } BigDBusWatchNameFlags;

typedef void (*BigDBusNameAppearedFunc)(DBusConnection* connection,
                                        const char* name,
                                        const char* new_owner_unique_name,
                                        void* data);
typedef void (*BigDBusNameVanishedFunc)(DBusConnection* connection,
                                        const char* name,
                                        const char* old_owner_unique_name,
                                        void* data);

typedef struct
{
    BigDBusNameAppearedFunc appeared;
    BigDBusNameVanishedFunc vanished;
} BigDBusWatchNameFuncs;

void big_dbus_watch_name(DBusBusType bus_type,
                         const char* name,
                         BigDBusWatchNameFlags flags,
                         const BigDBusWatchNameFuncs* funcs,
                         void* data);
void big_dbus_unwatch_name(DBusBusType bus_type,
                           const char* name,
                           const BigDBusWatchNameFuncs* funcs,
                           void* data);
const char* big_dbus_get_watched_name_owner(DBusBusType bus_type,
                                            const char* name);

typedef void (*BigDBusSignalHandler)(DBusConnection* connection,
                                     DBusMessage* message,
                                     void* data);
int big_dbus_watch_signal(DBusBusType bus_type,
                          const char* sender,
                          const char* path,
                          const char* iface,
                          const char* name,
                          BigDBusSignalHandler handler,
                          void* data,
                          GDestroyNotify data_dnotify);
void big_dbus_unwatch_signal(DBusBusType bus_type,
                             const char* sender,
                             const char* path,
                             const char* iface,
                             const char* name,
                             BigDBusSignalHandler handler,
                             void* data);
void big_dbus_unwatch_signal_by_id(DBusBusType bus_type, int id);

/* A "json method" is a D-Bus method with signature
 *      DICT jsonMethodName(DICT)
 * with the idea that it both takes and returns
 * a JavaScript-style dictionary. This makes
 * our JavaScript-to-dbus bindings really simple,
 * and avoids a lot of futzing with dbus IDL.
 *
 * Of course it's completely annoying for someone
 * using D-Bus in a "normal" way but the idea is just
 * to use this to communicate within our own app
 * that happens to consist of multiple processes
 * and have bits written in JS.
 */
typedef void (*BigDBusJsonSyncMethodFunc)(DBusConnection* connection,
                                          DBusMessage* message,
                                          DBusMessageIter* in_iter,
                                          DBusMessageIter* out_iter,
                                          void* data,
                                          DBusError* error);

typedef void (*BigDBusJsonAsyncMethodFunc)(DBusConnection* connection,
                                           DBusMessage* message,
                                           DBusMessageIter* in_iter,
                                           void* data);

typedef struct
{
    const char* name;
    /* one of these two but not both should be non-NULL */
    BigDBusJsonSyncMethodFunc sync_func;
    BigDBusJsonAsyncMethodFunc async_func;
} BigDBusJsonMethod;

void big_dbus_register_json(DBusConnection* connection,
                            const char* iface_name,
                            const BigDBusJsonMethod* methods,
                            int n_methods);
void big_dbus_unregister_json(DBusConnection* connection,
                              const char* iface_name);
void big_dbus_register_g_object(DBusConnection* connection,
                                const char* path,
                                GObject* gobj,
                                const char* iface_name);
void big_dbus_unregister_g_object(DBusConnection* connection, const char* path);

void big_dbus_append_json_entry(DBusMessageIter* dict_iter,
                                const char* key,
                                int dbus_type,
                                void* basic_value_p);
void big_dbus_append_json_entry_STRING(DBusMessageIter* dict_iter,
                                       const char* key,
                                       const char* value);
void big_dbus_append_json_entry_INT32(DBusMessageIter* dict_iter,
                                      const char* key,
                                      dbus_int32_t value);
void big_dbus_append_json_entry_DOUBLE(DBusMessageIter* dict_iter,
                                       const char* key,
                                       double value);
void big_dbus_append_json_entry_BOOLEAN(DBusMessageIter* dict_iter,
                                        const char* key,
                                        dbus_bool_t value);
void big_dbus_append_json_entry_EMPTY_ARRAY(DBusMessageIter* dict_iter,
                                            const char* key);
void big_dbus_append_json_entry_STRING_ARRAY(DBusMessageIter* dict_iter,
                                             const char* key,
                                             const char** value);

gboolean big_dbus_message_iter_get_gsize(DBusMessageIter* iter, gsize* value_p);
gboolean big_dbus_message_iter_get_gssize(DBusMessageIter* iter,
                                          gssize* value_p);

void big_dbus_start_service(DBusConnection* connection, const char* name);

G_END_DECLS

#endif /* __BIG_UTIL_DBUS_H__ */
