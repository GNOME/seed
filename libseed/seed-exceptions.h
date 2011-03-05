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

#ifndef _SEED_EXCEPTIONS_H
#define _SEED_EXCEPTIONS_H

#include "seed-private.h"

void
seed_make_exception (JSContextRef ctx, JSValueRef * exception,
		     const gchar * name, const gchar * message, ...)
G_GNUC_PRINTF (4, 5);

     void seed_make_exception_from_gerror (JSContextRef ctx,
					   JSValueRef * exception,
					   GError * e);

     gchar *seed_exception_get_name (JSContextRef ctx, JSValueRef e);
     gchar *seed_exception_get_message (JSContextRef ctx, JSValueRef e);
     guint seed_exception_get_line (JSContextRef ctx, JSValueRef e);
     gchar *seed_exception_get_file (JSContextRef ctx, JSValueRef e);
     gchar *seed_exception_to_string (JSContextRef ctx, JSValueRef e);

#endif
