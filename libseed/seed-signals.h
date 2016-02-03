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

#ifndef _SEED_SIGNALS_H
#define _SEED_SIGNALS_H

#include "seed-private.h"

void seed_signal_marshal_func(GClosure* closure,
                              GValue* return_value,
                              guint n_param_values,
                              const GValue* param_values,
                              gpointer invocation_hint,
                              gpointer marshall_data);

void seed_add_signals_to_object(JSContextRef ctx,
                                JSObjectRef object_ref,
                                GObject* obj);

gulong seed_gobject_signal_connect(JSContextRef ctx,
                                   const gchar* signal_name,
                                   GObject* on_obj,
                                   JSObjectRef func,
                                   JSObjectRef this_obj,
                                   JSObjectRef user_data);

JSValueRef seed_gobject_signal_connect_by_name(JSContextRef ctx,
                                               JSObjectRef function,
                                               JSObjectRef thisObject,
                                               size_t argumentCount,
                                               const JSValueRef arguments[],
                                               JSValueRef* exception);

JSClassDefinition* seed_get_signal_class(void);
extern JSClassRef gobject_signal_class;

#endif
