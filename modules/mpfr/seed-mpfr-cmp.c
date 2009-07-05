
#include <mpfr.h>
#include "seed-mpfr.h"

SeedValue seed_mpfr_cmp (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_ptr rop, op;
    gboolean ret;

    CHECK_ARG_COUNT("mpfr.cmp", 1);

    rop = seed_object_get_private(this_object);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.cmp", "mpfr_t");
    }

    ret = mpfr_cmp(rop, op);

    return seed_value_from_boolean(ctx, ret, exception);
}

