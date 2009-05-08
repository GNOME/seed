/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/* Copyright 2008 litl, LLC. All Rights Reserved. */

#ifndef __BIG_UTIL_DBUS_PRIVATE_H__
#define __BIG_UTIL_DBUS_PRIVATE_H__

#include <glib.h>
#include <util/dbus.h>
#include <util/dbus-proxy.h>

G_BEGIN_DECLS

typedef struct {
    DBusBusType bus_type;
    void *where_connection_was;
    BigDBusProxy *driver_proxy;
    GHashTable *json_ifaces;
    GSList *name_ownership_monitors;
    GHashTable *name_watches;

    GSList *all_signal_watchers;

    /* These signal watcher tables are maps from a
     * string to a GSList of BigSignalWatcher,
     * and they are lazily created if a signal watcher
     * needs to be looked up by the given key.
     */
    GHashTable *signal_watchers_by_unique_sender;
    GHashTable *signal_watchers_by_path;
    GHashTable *signal_watchers_by_iface;
    GHashTable *signal_watchers_by_signal;
    /* These are matching on well-known name only,
     * or watching all signals
     */
    GSList     *signal_watchers_in_no_table;

} BigDBusInfo;

BigDBusInfo*      _big_dbus_ensure_info                     (DBusConnection *connection);
void              _big_dbus_dispose_info                    (DBusConnection *connection);
void              _big_dbus_process_pending_signal_watchers (DBusConnection *connection,
                                                             BigDBusInfo    *info);
DBusHandlerResult _big_dbus_signal_watch_filter_message     (DBusConnection *connection,
                                                             DBusMessage    *message,
                                                             void           *data);
void              _big_dbus_set_matching_name_owner_changed (DBusConnection *connection,
                                                             const char     *bus_name,
                                                             gboolean        matched);
void              _big_dbus_ensure_connect_idle             (DBusBusType     bus_type);
DBusConnection*   _big_dbus_get_weak_ref                    (DBusBusType     which_bus);


G_END_DECLS

#endif  /* __BIG_UTIL_DBUS_PRIVATE_H__ */
