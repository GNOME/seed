
#include <mpfr.h>
#include "seed-mpfr.h"

/* This is a bit disgusting. Oh well. */
SeedValue seed_mpfr_add (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gdouble dop1, dop2;
    gint ret;
    seed_mpfr_t argt1, argt2;
    /* only want 1 double argument. alternatively, could accept 2,
       add those, and set from the result*/

    CHECK_ARG_COUNT("mpfr.add", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    argt1 = seed_mpfr_arg_type(ctx, args[0], exception);
    argt2 = seed_mpfr_arg_type(ctx, args[1], exception);

    if ( (argt1 & argt2) == SEED_MPFR_MPFR )
    {
        /* both mpfr_t */
        op1 = seed_object_get_private(args[0]);
        op2 = seed_object_get_private(args[1]);
        ret = mpfr_add(rop, op1, op2, rnd);
    }
    else if ( (argt1 | argt2) == (SEED_MPFR_MPFR | SEED_MPFR_DOUBLE) )
    {
        /* a double and an mpfr_t. Figure out the order */
        if ( argt1 == SEED_MPFR_MPFR )
        {
            op1 = seed_object_get_private(args[0]);
            dop2 = seed_value_to_double(ctx, args[1], exception);
            mpfr_add_d(rop, op1, dop2, rnd);
        }
        else
        {
            dop2 = seed_value_to_double(ctx, args[0], exception);
            op1 = seed_object_get_private(args[1]);
            mpfr_add_d(rop, op1, dop2, rnd);
        }
    }
    else if ( (argt1 & argt2) == SEED_MPFR_DOUBLE )
    {
        /* 2 doubles. hopefully doesn't happen */
        dop1 = seed_value_to_double(ctx, args[0], exception);
        dop2 = seed_value_to_double(ctx, args[1], exception);
        ret = mpfr_set_d(rop, dop1 + dop2, rnd);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.add", "double or mpfr_t");
    }

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_sqrt (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.sqrt", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.sqrt", "mpfr_t");
    }

    ret = mpfr_sqrt(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_rec_sqrt (SeedContext ctx,
                              SeedObject function,
                              SeedObject this_object,
                              gsize argument_count,
                              const SeedValue args[],
                              SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.rec_sqrt", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.rec_sqrt", "mpfr_t");
    }

    ret = mpfr_rec_sqrt(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_cbrt (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.cbrt", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.cbrt", "mpfr_t");
    }

    ret = mpfr_cbrt(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_neg (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.neg", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.neg", "mpfr_t");
    }

    ret = mpfr_neg(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_abs (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.abs", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.abs", "mpfr_t");
    }

    ret = mpfr_abs(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_dim (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gint ret;

    CHECK_ARG_COUNT("mpfr.dim", 3);

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
        TYPE_EXCEPTION("mpfr.dim", "mpfr_t");
    }

    ret = mpfr_dim(rop, op1, op2, rnd);

    return seed_value_from_int(ctx, ret, exception);
}


/* Rounding functions */

SeedValue seed_mpfr_rint (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.rint", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.rint", "mpfr_t");
    }

    ret = mpfr_rint(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_ceil (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.ceil", 1);

    rop = seed_object_get_private(this_object);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.ceil", "mpfr_t");
    }

    ret = mpfr_ceil(rop, op);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_floor (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.floor", 1);

    rop = seed_object_get_private(this_object);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.floor", "mpfr_t");
    }

    ret = mpfr_floor(rop, op);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_round (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.round", 1);

    rop = seed_object_get_private(this_object);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.round", "mpfr_t");
    }

    ret = mpfr_round(rop, op);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_trunc (SeedContext ctx,
                           SeedObject function,
                           SeedObject this_object,
                           gsize argument_count,
                           const SeedValue args[],
                           SeedException * exception)
{
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.trunc", 1);

    rop = seed_object_get_private(this_object);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.trunc", "mpfr_t");
    }

    ret = mpfr_trunc(rop, op);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_rint_ceil (SeedContext ctx,
                               SeedObject function,
                               SeedObject this_object,
                               gsize argument_count,
                               const SeedValue args[],
                               SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.rint_ceil", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.rint_ceil", "mpfr_t");
    }

    ret = mpfr_rint_ceil(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_rint_floor (SeedContext ctx,
                                SeedObject function,
                                SeedObject this_object,
                                gsize argument_count,
                                const SeedValue args[],
                                SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.rint_floor", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.rint_floor", "mpfr_t");
    }

    ret = mpfr_rint_floor(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_rint_round (SeedContext ctx,
                                SeedObject function,
                                SeedObject this_object,
                                gsize argument_count,
                                const SeedValue args[],
                                SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.rint_round", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.rint_round", "mpfr_t");
    }

    ret = mpfr_rint_round(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_rint_trunc (SeedContext ctx,
                                SeedObject function,
                                SeedObject this_object,
                                gsize argument_count,
                                const SeedValue args[],
                                SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.rint_trunc", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.rint_trunc", "mpfr_t");
    }

    ret = mpfr_rint_trunc(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_frac (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.frac", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.frac", "mpfr_t");
    }

    ret = mpfr_frac(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}


SeedValue seed_mpfr_integer_p (SeedContext ctx,
                               SeedObject function,
                               SeedObject this_object,
                               gsize argument_count,
                               const SeedValue args[],
                               SeedException * exception)
{
    mpfr_ptr rop;
    gboolean ret;

    CHECK_ARG_COUNT("mpfr.integer", 0);
    rop = seed_object_get_private(this_object);
    ret = mpfr_integer_p(rop);

    return seed_value_from_boolean(ctx, ret, exception);
}

SeedValue seed_mpfr_fmod (SeedContext ctx,
                          SeedObject function,
                          SeedObject this_object,
                          gsize argument_count,
                          const SeedValue args[],
                          SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gint ret;

    CHECK_ARG_COUNT("mpfr.fmod", 3);

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
        TYPE_EXCEPTION("mpfr.fmod", "mpfr_t");
    }

    ret = mpfr_fmod(rop, op1, op2, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_remainder (SeedContext ctx,
                               SeedObject function,
                               SeedObject this_object,
                               gsize argument_count,
                               const SeedValue args[],
                               SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gint ret;

    CHECK_ARG_COUNT("mpfr.remainder", 3);

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
        TYPE_EXCEPTION("mpfr.remainder", "mpfr_t");
    }

    ret = mpfr_remainder(rop, op1, op2, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

