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

#ifndef _SEED_PRIVATE_H
#define _SEED_PRIVATE_H

#include <sys/types.h>
#include <pthread.h>

#include <stdlib.h>
#include <JavaScriptCore/JavaScript.h>
#include <glib.h>
#include <glib-object.h>
#include <gmodule.h>
#include <girepository.h>
#include <girffi.h>
#include <ffi.h>

typedef struct _SeedEngine SeedEngine;

struct _SeedEngine
{
    JSGlobalContextRef context;
    JSObjectRef global;
    gchar** search_path;

    JSContextGroupRef group;
    gchar* program_name;
};

#include "seed-debug.h"
#include "seed-engine.h"
#include "seed-types.h"
#include "seed-signals.h"
#include "seed-builtins.h"
#include "seed-structs.h"
#include "seed-closure.h"
#include "seed-gtype.h"
#include "seed-exceptions.h"
#include "seed-importer.h"

#endif
