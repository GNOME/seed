
#ifndef __SEED_JS_DBUS_EXPORTS_H__
#define __SEED_JS_DBUS_EXPORTS_H__

#include <glib.h>
#include <dbus/dbus.h>
#include <seed.h>

G_BEGIN_DECLS

gboolean seed_js_define_dbus_exports (SeedContext ctx,
				      SeedObject object,
				      DBusBusType which_bus);

G_END_DECLS

#endif
