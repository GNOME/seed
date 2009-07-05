
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

SeedValue seed_mpfr_cmpabs (SeedContext ctx,
                            SeedObject function,
                            SeedObject this_object,
                            gsize argument_count,
                            const SeedValue args[],
                            SeedException * exception)
{
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.cmpabs", 1);

    rop = seed_object_get_private(this_object);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.cmpabs", "mpfr_t");
    }

    ret = mpfr_cmpabs(rop, op);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue
seed_mpfr_nan_p (SeedContext ctx,
                 SeedObject function,
                 SeedObject this_object,
                 gsize argument_count,
                 const SeedValue args[],
                 SeedException * exception)
{
    mpfr_ptr rop;
    gboolean ret;

    CHECK_ARG_COUNT("mpfr.nan_p", 0);

    rop = seed_object_get_private(this_object);

    ret = mpfr_nan_p(rop);

    return seed_value_from_boolean(ctx, ret, exception);
}

SeedValue
seed_mpfr_inf_p (SeedContext ctx,
                 SeedObject function,
                 SeedObject this_object,
                 gsize argument_count,
                 const SeedValue args[],
                 SeedException * exception)
{
    mpfr_ptr rop;
    gboolean ret;

    CHECK_ARG_COUNT("mpfr.inf_p", 0);

    rop = seed_object_get_private(this_object);

    ret = mpfr_inf_p(rop);

    return seed_value_from_boolean(ctx, ret, exception);
}

SeedValue
seed_mpfr_number_p (SeedContext ctx,
                    SeedObject function,
                    SeedObject this_object,
                    gsize argument_count,
                    const SeedValue args[],
                    SeedException * exception)
{
    mpfr_ptr rop;
    gboolean ret;

    CHECK_ARG_COUNT("mpfr.number_p", 0);

    rop = seed_object_get_private(this_object);

    ret = mpfr_number_p(rop);

    return seed_value_from_boolean(ctx, ret, exception);
}

SeedValue
seed_mpfr_zero_p (SeedContext ctx,
                  SeedObject function,
                  SeedObject this_object,
                  gsize argument_count,
                  const SeedValue args[],
                  SeedException * exception)
{
    mpfr_ptr rop;
    gboolean ret;

    CHECK_ARG_COUNT("mpfr.zero_p", 0);

    rop = seed_object_get_private(this_object);

    ret = mpfr_zero_p(rop);

    return seed_value_from_boolean(ctx, ret, exception);
}

SeedValue seed_mpfr_greater_p (SeedContext ctx,
                               SeedObject function,
                               SeedObject this_object,
                               gsize argument_count,
                               const SeedValue args[],
                               SeedException * exception)
{
    mpfr_ptr rop, op;
    gboolean ret;

    CHECK_ARG_COUNT("mpfr.greater_p", 1);

    rop = seed_object_get_private(this_object);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.greater_p", "mpfr_t");
    }

    ret = mpfr_greater_p(rop, op);

    return seed_value_from_boolean(ctx, ret, exception);
}

