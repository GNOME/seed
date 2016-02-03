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
 * Copyright (C) Alan Knowles 2011 <alan@akbkhome.com>
 *
 * This started of as a way to get Xorg information not easily available
 * elsewhere
 *
 * It currently is only used with libxss (screensaver extension) to
 * get the idle time.
 *
 */

#include "../../libseed/seed.h"
#include <X11/extensions/scrnsaver.h>

/**
 * screensaverinfo_get_idletime()
 * @returns screen idle time.
 */

SeedValue
seed_xorg_screensaverinfo_get_idletime(SeedContext ctx,
                                       SeedObject function,
                                       SeedObject this_object,
                                       size_t argument_count,
                                       const SeedValue arguments[],
                                       SeedException* exception)
{
    int rc = -1;

    XScreenSaverInfo* info = XScreenSaverAllocInfo();
    Display* display = XOpenDisplay(NULL);

    if (display != NULL) {
        XScreenSaverQueryInfo(display, DefaultRootWindow(display), info);
        rc = info->idle;
        XCloseDisplay(display);
    }

    XFree(info);
    return seed_value_from_int(ctx, rc, exception);
}

seed_static_function xorg_funcs[]
  = { { "screensaverinfo_get_idletime", seed_xorg_screensaverinfo_get_idletime,
        0 },

      { NULL, NULL, 0 } };

SeedObject
seed_module_init(SeedEngine* eng)
{
    SeedObject seed_xorg;
    SeedClass seed_xorg_class;
    seed_class_definition seed_xorg_class_definition = seed_empty_class;

    seed_xorg_class_definition.static_functions = xorg_funcs;
    seed_xorg_class = seed_create_class(&seed_xorg_class_definition);

    seed_xorg = seed_make_object(eng->context, seed_xorg_class, NULL);

    return seed_xorg;
}
