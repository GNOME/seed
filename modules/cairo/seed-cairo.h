#ifndef _SEED_CAIRO_H
#define _SEED_CAIRO_H
#include <seed.h>
#include <cairo/cairo.h>

#define EXPECTED_EXCEPTION(name, argnum)                                       \
    seed_make_exception(ctx, exception, "ArgumentError",                       \
                        name " expected " argnum " got %zd", argument_count);  \
    return seed_make_undefined(ctx);

cairo_user_data_key_t* seed_get_cairo_key();
void seed_cairo_destroy_func(void* obj);

#endif
