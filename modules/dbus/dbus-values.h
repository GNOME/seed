/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/* Copyright 2008 litl, LLC. All Rights Reserved. */

#ifndef __SEED_JS_DBUS_VALUES_H__
#define __SEED_JS_DBUS_VALUES_H__

#include <glib.h>
#include <dbus/dbus.h>
#include <seed.h>

G_BEGIN_DECLS

extern SeedContextGroup group;

gboolean seed_js_values_from_dbus    (SeedContext          ctx,
                                      DBusMessageIter    *iter,
                                      GArray **aray_p,
                                      SeedException *exception);
gboolean seed_js_one_value_from_dbus (SeedContext          ctx,
                                      DBusMessageIter    *iter,
                                      SeedValue              *value_p,
                                      SeedException *exception);
gboolean seed_js_values_to_dbus      (SeedContext          ctx,
                                      int                 index,
                                      SeedObject               values,
                                      DBusMessageIter    *iter,
                                      DBusSignatureIter  *sig_iter,
                                      SeedException *exception);
gboolean seed_js_one_value_to_dbus   (SeedContext          ctx,
                                      SeedValue               value,
                                      DBusMessageIter    *iter,
                                      DBusSignatureIter  *sig_iter,
                                      SeedException *exception);
gboolean seed_js_add_dbus_props      (SeedContext          ctx,
                                      DBusMessage        *message,
                                      SeedValue               value,
                                      SeedException *exception);


G_END_DECLS

#endif  /* __SEED_JS_DBUS_VALUES_H__ */
