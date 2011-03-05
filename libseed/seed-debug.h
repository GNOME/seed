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

#ifndef _SEED_DEBUG_H
#define _SEED_DEBUG_H

// Borrowed from Clutter, more or less.

#include <glib.h>

typedef enum
{
  SEED_DEBUG_ALL = 1 << 0,
  SEED_DEBUG_MISC = 1 << 1,
  SEED_DEBUG_FINALIZATION = 1 << 2,
  SEED_DEBUG_INITIALIZATION = 1 << 3,
  SEED_DEBUG_CONSTRUCTION = 1 << 4,
  SEED_DEBUG_INVOCATION = 1 << 5,
  SEED_DEBUG_SIGNAL = 1 << 6,
  SEED_DEBUG_STRUCTS = 1 << 7,
  SEED_DEBUG_GTYPE = 1 << 8,
  SEED_DEBUG_IMPORTER = 1 << 9,
  SEED_DEBUG_MODULE = 1 << 10
} SeedDebugFlag;

#ifdef SEED_ENABLE_DEBUG

#define SEED_NOTE(type,...)  G_STMT_START {                 \
    if ((seed_debug_flags & SEED_DEBUG_##type) ||           \
        seed_debug_flags & SEED_DEBUG_ALL)                  \
    {                                                       \
        gchar * _fmt = g_strdup_printf (__VA_ARGS__);       \
        g_message ("[" #type "] " G_STRLOC ": %s",_fmt);    \
        g_free (_fmt);                                      \
    }                                                       \
} G_STMT_END

#define SEED_MARK()      SEED_NOTE(MISC, "== mark ==")
#define SEED_DBG(x) { a }

#else /* !SEED_ENABLE_DEBUG */

#define SEED_NOTE(type,...)
#define SEED_MARK()
#define SEED_DBG(x)

#endif /* SEED_ENABLE_DEBUG */

extern guint seed_debug_flags;

#endif
