
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


/* log functions */

SeedValue seed_mpfr_log (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.log", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.log", "mpfr_t");
    }

    ret = mpfr_log(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_log2 (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.log2", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.log2", "mpfr_t");
    }

    ret = mpfr_log2(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_log10 (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.log10", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.log10", "mpfr_t");
    }

    ret = mpfr_log10(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

/* hyperbolic trig functions */

SeedValue seed_mpfr_sinh (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.sinh", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.sinh", "mpfr_t");
    }

    ret = mpfr_sinh(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_cosh (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.cosh", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.cosh", "mpfr_t");
    }

    ret = mpfr_cosh(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_tanh (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.tanh", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.tanh", "mpfr_t");
    }

    ret = mpfr_tanh(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}


SeedValue seed_mpfr_sech (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.sech", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.sech", "mpfr_t");
    }

    ret = mpfr_sech(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_csch (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.csch", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.csch", "mpfr_t");
    }

    ret = mpfr_csch(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_coth (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.coth", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.coth", "mpfr_t");
    }

    ret = mpfr_coth(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

/* inverse hyperbolic trig */

SeedValue seed_mpfr_asinh (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.asinh", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.asinh", "mpfr_t");
    }

    ret = mpfr_asinh(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_acosh (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.acosh", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.acosh", "mpfr_t");
    }

    ret = mpfr_acosh(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_atanh (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.atanh", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.atanh", "mpfr_t");
    }

    ret = mpfr_atanh(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_log1p (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.log1p", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.log1p", "mpfr_t");
    }

    ret = mpfr_log1p(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_expm1 (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.expm1", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.expm1", "mpfr_t");
    }

    ret = mpfr_expm1(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_li2 (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.li2", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.li2", "mpfr_t");
    }

    ret = mpfr_li2(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_gamma (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.gamma", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.gamma", "mpfr_t");
    }

    ret = mpfr_gamma(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_lngamma (SeedContext ctx,
                             SeedObject function,
                             SeedObject this_object,
                             gsize argument_count,
                             const SeedValue args[],
                             SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.lngamma", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.lngamma", "mpfr_t");
    }

    ret = mpfr_lngamma(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_zeta (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.zeta", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.zeta", "mpfr_t");
    }

    ret = mpfr_zeta(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_erf (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.erf", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.erf", "mpfr_t");
    }

    ret = mpfr_erf(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_erfc (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.erfc", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.erfc", "mpfr_t");
    }

    ret = mpfr_erfc(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

/* bessel functions */
SeedValue seed_mpfr_j0 (SeedContext ctx,
                        SeedObject function,
                        SeedObject this_object,
                        gsize argument_count,
                        const SeedValue args[],
                        SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.j0", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.j0", "mpfr_t");
    }

    ret = mpfr_j0(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_j1 (SeedContext ctx,
                        SeedObject function,
                        SeedObject this_object,
                        gsize argument_count,
                        const SeedValue args[],
                        SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.j1", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.j1", "mpfr_t");
    }

    ret = mpfr_j1(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_jn (SeedContext ctx,
                        SeedObject function,
                        SeedObject this_object,
                        gsize argument_count,
                        const SeedValue args[],
                        SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret, n;

    CHECK_ARG_COUNT("mpfr.jn", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    if ( seed_value_is_object_of_class(ctx, args[1], mpfr_class) )
    {
        op = seed_object_get_private(args[1]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.jn", "mpfr_t");
    }

    if ( seed_value_is_number(ctx, args[0]) )
    {
        n = seed_value_to_int(ctx, args[0], exception);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.jn", "int");
    }

    ret = mpfr_jn(rop, n, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_y0 (SeedContext ctx,
                        SeedObject function,
                        SeedObject this_object,
                        gsize argument_count,
                        const SeedValue args[],
                        SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.y0", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.y0", "mpfr_t");
    }

    ret = mpfr_y0(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_y1 (SeedContext ctx,
                        SeedObject function,
                        SeedObject this_object,
                        gsize argument_count,
                        const SeedValue args[],
                        SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.y1", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.y1", "mpfr_t");
    }

    ret = mpfr_y1(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_yn (SeedContext ctx,
                        SeedObject function,
                        SeedObject this_object,
                        gsize argument_count,
                        const SeedValue args[],
                        SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret, n;

    CHECK_ARG_COUNT("mpfr.yn", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    if ( seed_value_is_object_of_class(ctx, args[1], mpfr_class) )
    {
        op = seed_object_get_private(args[1]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.yn", "mpfr_t");
    }

    if ( seed_value_is_number(ctx, args[0]) )
    {
        n = seed_value_to_int(ctx, args[0], exception);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.yn", "int");
    }

    ret = mpfr_yn(rop, n, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_fma (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2, op3;
    gint ret;

    CHECK_ARG_COUNT("mpfr.fma", 4);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[3], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) &&
         seed_value_is_object_of_class(ctx, args[1], mpfr_class) &&
         seed_value_is_object_of_class(ctx, args[2], mpfr_class))
    {
        op1 = seed_object_get_private(args[0]);
        op2 = seed_object_get_private(args[1]);
        op3 = seed_object_get_private(args[2]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.fma", "mpfr_t");
    }

    ret = mpfr_fma(rop, op1, op2, op3, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_fms (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2, op3;
    gint ret;

    CHECK_ARG_COUNT("mpfr.fms", 4);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[3], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) &&
         seed_value_is_object_of_class(ctx, args[1], mpfr_class) &&
         seed_value_is_object_of_class(ctx, args[2], mpfr_class))
    {
        op1 = seed_object_get_private(args[0]);
        op2 = seed_object_get_private(args[1]);
        op3 = seed_object_get_private(args[2]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.fms", "mpfr_t");
    }

    ret = mpfr_fms(rop, op1, op2, op3, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_agm (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gint ret;

    CHECK_ARG_COUNT("mpfr.agm", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) &&
         seed_value_is_object_of_class(ctx, args[1], mpfr_class))
    {
        op1 = seed_object_get_private(args[0]);
        op2 = seed_object_get_private(args[1]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.agm", "mpfr_t");
    }

    ret = mpfr_agm(rop, op1, op2, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_hypot (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gint ret;

    CHECK_ARG_COUNT("mpfr.hypot", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) &&
         seed_value_is_object_of_class(ctx, args[1], mpfr_class))
    {
        op1 = seed_object_get_private(args[0]);
        op2 = seed_object_get_private(args[1]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.hypot", "mpfr_t");
    }

    ret = mpfr_hypot(rop, op1, op2, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_free_cache (SeedContext ctx,
                                SeedObject function,
                                SeedObject this_object,
                                gsize argument_count,
                                const SeedValue args[],
                                SeedException * exception)
{
    CHECK_ARG_COUNT("mpfr.free_cache", 0);
    mpfr_free_cache();
    return seed_make_null(ctx);
}

