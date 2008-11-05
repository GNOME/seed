/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-builtins.h
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

#ifndef _SEED_BUILTINS_H_
#define _SEED_BUILTINS_H_

#include "seed-private.h"

// TODO: someday, maybe, move import_namespace here!


JSValueRef
seed_include(JSContextRef ctx,
	     JSObjectRef function,
	     JSObjectRef this_object,
	     size_t argumentCount,
	     const JSValueRef arguments[], JSValueRef * exception);

JSValueRef
seed_print(JSContextRef ctx,
	   JSObjectRef function,
	   JSObjectRef this_object,
	   size_t argumentCount,
	   const JSValueRef arguments[], JSValueRef * exception);

JSValueRef
seed_readline(JSContextRef ctx,
	      JSObjectRef function,
	      JSObjectRef this_object,
	      size_t argumentCount,
	      const JSValueRef arguments[], JSValueRef * exception);

JSValueRef
seed_introspect(JSContextRef ctx,
		JSObjectRef function,
		JSObjectRef this_object,
		size_t argumentCount,
		const JSValueRef arguments[], JSValueRef * exception);

void seed_init_builtins();

#endif
