/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
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

SeedValue seed_mpfr_sub (SeedContext ctx,
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

    CHECK_ARG_COUNT("mpfr.sub", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    argt1 = seed_mpfr_arg_type(ctx, args[0], exception);
    argt2 = seed_mpfr_arg_type(ctx, args[1], exception);

    if ( (argt1 & argt2) == SEED_MPFR_MPFR )
    {
        /* both mpfr_t */
        op1 = seed_object_get_private(args[0]);
        op2 = seed_object_get_private(args[1]);
        ret = mpfr_sub(rop, op1, op2, rnd);
    }
    else if ( (argt1 | argt2) == (SEED_MPFR_MPFR | SEED_MPFR_DOUBLE) )
    {
        /* a double and an mpfr_t. Figure out the order */
        if ( argt1 == SEED_MPFR_MPFR )
        {
            op1 = seed_object_get_private(args[0]);
            dop2 = seed_value_to_double(ctx, args[1], exception);
            mpfr_sub_d(rop, op1, dop2, rnd);
        }
        else
        {
            dop1 = seed_value_to_double(ctx, args[0], exception);
            op2 = seed_object_get_private(args[1]);
            mpfr_d_sub(rop, dop1, op2, rnd);
        }
    }
    else if ( (argt1 & argt2) == SEED_MPFR_DOUBLE )
    {
        /* 2 doubles. hopefully doesn't happen */
        dop1 = seed_value_to_double(ctx, args[0], exception);
        dop2 = seed_value_to_double(ctx, args[1], exception);
        ret = mpfr_set_d(rop, dop1 - dop2, rnd);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.sub", "double or mpfr_t");
    }

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_mul (SeedContext ctx,
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

    CHECK_ARG_COUNT("mpfr.mul", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    argt1 = seed_mpfr_arg_type(ctx, args[0], exception);
    argt2 = seed_mpfr_arg_type(ctx, args[1], exception);

    if ( (argt1 & argt2) == SEED_MPFR_MPFR )
    {
        /* both mpfr_t */
        op1 = seed_object_get_private(args[0]);
        op2 = seed_object_get_private(args[1]);
        ret = mpfr_mul(rop, op1, op2, rnd);
    }
    else if ( (argt1 | argt2) == (SEED_MPFR_MPFR | SEED_MPFR_DOUBLE) )
    {
        /* a double and an mpfr_t. Figure out the order */
        if ( argt1 == SEED_MPFR_MPFR )
        {
            op1 = seed_object_get_private(args[0]);
            dop2 = seed_value_to_double(ctx, args[1], exception);
            mpfr_mul_d(rop, op1, dop2, rnd);
        }
        else
        {
            dop2 = seed_value_to_double(ctx, args[0], exception);
            op1 = seed_object_get_private(args[1]);
            mpfr_mul_d(rop, op1, dop2, rnd);
        }
    }
    else if ( (argt1 & argt2) == SEED_MPFR_DOUBLE )
    {
        /* 2 doubles. hopefully doesn't happen */
        dop1 = seed_value_to_double(ctx, args[0], exception);
        dop2 = seed_value_to_double(ctx, args[1], exception);
        ret = mpfr_set_d(rop, dop1 * dop2, rnd);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.mul", "double or mpfr_t");
    }

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_div (SeedContext ctx,
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

    CHECK_ARG_COUNT("mpfr.div", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    argt1 = seed_mpfr_arg_type(ctx, args[0], exception);
    argt2 = seed_mpfr_arg_type(ctx, args[1], exception);

    if ( (argt1 & argt2) == SEED_MPFR_MPFR )
    {
        /* both mpfr_t */
        op1 = seed_object_get_private(args[0]);
        op2 = seed_object_get_private(args[1]);
        ret = mpfr_div(rop, op1, op2, rnd);
    }
    else if ( (argt1 | argt2) == (SEED_MPFR_MPFR | SEED_MPFR_DOUBLE) )
    {
        /* a double and an mpfr_t. Figure out the order */
        if ( argt1 == SEED_MPFR_MPFR )
        {
            op1 = seed_object_get_private(args[0]);
            dop2 = seed_value_to_double(ctx, args[1], exception);
            mpfr_div_d(rop, op1, dop2, rnd);
        }
        else
        {
            dop1 = seed_value_to_double(ctx, args[0], exception);
            op2 = seed_object_get_private(args[1]);
            mpfr_d_div(rop, dop1, op2, rnd);
        }
    }
    else if ( (argt1 & argt2) == SEED_MPFR_DOUBLE )
    {
        /* 2 doubles. hopefully doesn't happen */
        dop1 = seed_value_to_double(ctx, args[0], exception);
        dop2 = seed_value_to_double(ctx, args[1], exception);
        ret = mpfr_set_d(rop, dop1 / dop2, rnd);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.div", "double or mpfr_t");
    }

    return seed_value_from_int(ctx, ret, exception);
}


SeedValue seed_mpfr_pow (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gint ret;
    glong iop;
    gulong uiop1, uiop2;
    seed_mpfr_t argt1, argt2;
    /* only want 1 double argument. alternatively, could accept 2,
       add those, and set from the result*/

    CHECK_ARG_COUNT("mpfr.pow", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    argt1 = seed_mpfr_arg_type(ctx, args[0], exception);
    argt2 = seed_mpfr_arg_type(ctx, args[1], exception);

    if ( (argt1 & argt2) == SEED_MPFR_MPFR )
    {
        /* both mpfr_t */
        op1 = seed_object_get_private(args[0]);
        op2 = seed_object_get_private(args[1]);
        ret = mpfr_pow(rop, op1, op2, rnd);
    }
    else if ( (argt1 | argt2) == (SEED_MPFR_MPFR | SEED_MPFR_DOUBLE) )
    {
        /* a double and an mpfr_t. Figure out the order */
        /* FIXME: is this switching ui and si bad? si_pow doesn't exist,
           and it's all from double anyway */
        if ( argt1 == SEED_MPFR_MPFR )
        {
            op1 = seed_object_get_private(args[0]);
            iop = seed_value_to_long(ctx, args[1], exception);
            ret = mpfr_pow_si(rop, op1, iop, rnd);
        }
        else
        {
            uiop1 = seed_value_to_ulong(ctx, args[0], exception);
            op2 = seed_object_get_private(args[1]);
            ret = mpfr_ui_pow(rop, uiop1, op2, rnd);
        }
    }
    else if ( (argt1 & argt2) == SEED_MPFR_DOUBLE )
    {
        /* pretend both ui */
        uiop1 = seed_value_to_ulong(ctx, args[0], exception);
        uiop2 = seed_value_to_ulong(ctx, args[1], exception);
        ret = mpfr_ui_pow_ui(rop, uiop1, uiop2, rnd);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.pow", "int or unsigned int and mpfr_t");
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

SeedValue seed_mpfr_sqr (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;

    CHECK_ARG_COUNT("mpfr.sqr", 2);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.sqr", "mpfr_t");
    }

    ret = mpfr_sqr(rop, op, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_root (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;
    gulong k;

    CHECK_ARG_COUNT("mpfr.root", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.root", "mpfr_t");
    }

    if ( seed_value_is_number(ctx, args[1]) )
    {
        k = seed_value_to_ulong(ctx, args[1], exception);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.root", "unsigned long int");
    }


    ret = mpfr_root(rop, op, k, rnd);

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

SeedValue seed_mpfr_nexttoward (SeedContext ctx,
                                SeedObject function,
                                SeedObject this_object,
                                gsize argument_count,
                                const SeedValue args[],
                                SeedException * exception)
{
    mpfr_ptr rop, op;

    CHECK_ARG_COUNT("mpfr.nexttoward", 1);

    rop = seed_object_get_private(this_object);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.nexttoward", "mpfr_t");
    }

    mpfr_nexttoward(rop, op);

    return seed_make_null(ctx);
}

SeedValue seed_mpfr_nextabove (SeedContext ctx,
                               SeedObject function,
                               SeedObject this_object,
                               gsize argument_count,
                               const SeedValue args[],
                               SeedException * exception)
{
    mpfr_ptr rop;

    CHECK_ARG_COUNT("mpfr.nextabove", 0);

    rop = seed_object_get_private(this_object);

    mpfr_nextabove(rop);
    return seed_make_null(ctx);
}

SeedValue seed_mpfr_nextbelow (SeedContext ctx,
                               SeedObject function,
                               SeedObject this_object,
                               gsize argument_count,
                               const SeedValue args[],
                               SeedException * exception)
{
    mpfr_ptr rop;

    CHECK_ARG_COUNT("mpfr.nextbelow", 0);

    rop = seed_object_get_private(this_object);

    mpfr_nextbelow(rop);
    return seed_make_null(ctx);
}

SeedValue seed_mpfr_min (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gint ret;

    CHECK_ARG_COUNT("mpfr.min", 3);

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
        TYPE_EXCEPTION("mpfr.min", "mpfr_t");
    }

    ret = mpfr_min(rop, op1, op2, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_max (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gint ret;

    CHECK_ARG_COUNT("mpfr.max", 3);

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
        TYPE_EXCEPTION("mpfr.max", "mpfr_t");
    }

    ret = mpfr_max(rop, op1, op2, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_prec_round (SeedContext ctx,
                                SeedObject function,
                                SeedObject this_object,
                                gsize argument_count,
                                const SeedValue args[],
                                SeedException * exception)
{
    mpfr_rnd_t rnd;
    mp_prec_t prec;
    mpfr_ptr rop;
    gint ret;

    CHECK_ARG_COUNT("mpfr.prec_round", 2);

    rop = seed_object_get_private(this_object);
    prec = seed_value_to_mpfr_prec_t(ctx, args[0], exception);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    ret = mpfr_prec_round(rop, prec, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_signbit (SeedContext ctx,
                             SeedObject function,
                             SeedObject this_object,
                             gsize argument_count,
                             const SeedValue args[],
                             SeedException * exception)
{
    mpfr_ptr rop;
    gboolean ret;

    CHECK_ARG_COUNT("mpfr.signbit", 0);
    rop = seed_object_get_private(this_object);
    ret = mpfr_signbit(rop);

    return seed_value_from_boolean(ctx, ret, exception);
}

SeedValue seed_mpfr_setsign (SeedContext ctx,
                             SeedObject function,
                             SeedObject this_object,
                             gsize argument_count,
                             const SeedValue args[],
                             SeedException * exception)
{
    mpfr_ptr rop, op;
    gint ret;
    gint s;
    mpfr_rnd_t rnd;

    CHECK_ARG_COUNT("mpfr.signbit", 3);

    rop = seed_object_get_private(this_object);
    s = seed_value_to_int(ctx, args[1], exception);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.setsign", "mpfr_t");
    }

    ret = mpfr_setsign(rop, op, s, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_copysign (SeedContext ctx,
                              SeedObject function,
                              SeedObject this_object,
                              gsize argument_count,
                              const SeedValue args[],
                              SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gint ret;

    CHECK_ARG_COUNT("mpfr.copysign", 3);

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
        TYPE_EXCEPTION("mpfr.copysign", "mpfr_t");
    }

    ret = mpfr_copysign(rop, op1, op2, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_mul_2si (SeedContext ctx,
                             SeedObject function,
                             SeedObject this_object,
                             gsize argument_count,
                             const SeedValue args[],
                             SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;
    gulong k;

    CHECK_ARG_COUNT("mpfr.mul_2si", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.mul_2si", "mpfr_t");
    }

    if ( seed_value_is_number(ctx, args[1]) )
    {
        k = seed_value_to_ulong(ctx, args[1], exception);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.mul_2si", "long int");
    }

    ret = mpfr_mul_2si(rop, op, k, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

SeedValue seed_mpfr_div_2si (SeedContext ctx,
                             SeedObject function,
                             SeedObject this_object,
                             gsize argument_count,
                             const SeedValue args[],
                             SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gint ret;
    gulong k;

    CHECK_ARG_COUNT("mpfr.div_2si", 3);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[2], exception);

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        op = seed_object_get_private(args[0]);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.div_2si", "mpfr_t");
    }

    if ( seed_value_is_number(ctx, args[1]) )
    {
        k = seed_value_to_ulong(ctx, args[1], exception);
    }
    else
    {
        TYPE_EXCEPTION("mpfr.div_2si", "long int");
    }

    ret = mpfr_div_2si(rop, op, k, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

