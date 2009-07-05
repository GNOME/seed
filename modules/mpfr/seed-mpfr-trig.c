
#include <mpfr.h>

#include "seed-mpfr.h"

SeedValue seed_mpfr_sin (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.sin", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.sin", "mpfr_t");
    }

    ret = mpfr_sin(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_cos (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.cos", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.cos", "mpfr_t");
    }

    ret = mpfr_cos(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_tan (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.tan", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.tan", "mpfr_t");
    }

    ret = mpfr_tan(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_csc (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.csc", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.csc", "mpfr_t");
    }

    ret = mpfr_csc(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_sec (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.sec", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.sec", "mpfr_t");
    }

    ret = mpfr_sec(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_cot (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.cot", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.cot", "mpfr_t");
    }

    ret = mpfr_cot(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_asin (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.asin", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.asin", "mpfr_t");
    }

    ret = mpfr_asin(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}


SeedValue seed_mpfr_acos (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.acos", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.acos", "mpfr_t");
    }

    ret = mpfr_acos(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_atan (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.atan", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.atan", "mpfr_t");
    }

    ret = mpfr_atan(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}
