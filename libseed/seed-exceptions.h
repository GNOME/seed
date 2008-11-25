/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * seed-engine.h
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 *
 * libseed is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libseed is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _SEED_EXCEPTIONS_H
#define _SEED_EXCEPTIONS_H

#include "seed-private.h"

void seed_make_exception(JSContextRef ctx, JSValueRef * exception,
						 const gchar * name, const gchar * message);

void seed_make_exception_from_gerror(JSContextRef ctx,
									 JSValueRef * exception, GError * e);

gchar *seed_exception_get_name(JSContextRef ctx, JSValueRef e);
gchar *seed_exception_get_message(JSContextRef ctx, JSValueRef e);
guint seed_exception_get_line(JSContextRef ctx, JSValueRef e);
gchar *seed_exception_get_file(JSContextRef ctx, JSValueRef e);
gchar *seed_exception_to_string(JSContextRef ctx, JSValueRef e);

#endif
