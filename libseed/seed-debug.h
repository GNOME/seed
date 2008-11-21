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
	SEED_DEBUG_SIGNAL = 1 << 4
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
