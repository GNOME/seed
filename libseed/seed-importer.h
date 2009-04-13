#ifndef _SEED_IMPORTER_H
#define _SEED_IMPORTER_H

#include "seed-private.h"

extern JSObjectRef importer;

void seed_initialize_importer (JSContextRef ctx, JSObjectRef global);

#endif
