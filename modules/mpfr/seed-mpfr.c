#include <seed-module.h>

#include <stdio.h>
#include <mpfr.h>

#include "seed-mpfr.h"

/* For now at least ignoring the ability to use gmp types since there is no gmp module */

SeedEngine * eng;

inline seed_mpfr_t
seed_mpfr_arg_type(SeedContext ctx, SeedValue arg, SeedException exept)
{
    if ( seed_value_is_object_of_class(ctx, arg, mpfr_class) )
        return SEED_MPFR_MPFR;
    else if ( seed_value_is_number(ctx, arg) )
        return SEED_MPFR_DOUBLE;
    else if ( seed_value_is_string(ctx, arg) )
        return SEED_MPFR_STRING;
    else
        return SEED_MPFR_UNKNOWN;
}

static SeedValue
seed_mpfr_out_str (SeedContext ctx,
                   SeedObject function,
                   SeedObject this_object,
                   gsize argument_count,
                   const SeedValue args[],
                   SeedException * exception)
{
    gsize n;
    FILE* stream;
    gint base;
    mpfr_rnd_t rnd;
    mpfr_ptr op;

    CHECK_ARG_COUNT("mpfr.out_str", 4);

    stream = (FILE*) seed_pointer_get_pointer(ctx, args[0]);
    base = seed_value_to_int(ctx, args[1], exception);
    n = seed_value_to_uint(ctx, args[2], exception);
    op = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[3], exception);

    return seed_value_from_uint(ctx,
                                mpfr_out_str(stream, base, n, op, rnd),
                                exception);
}


static SeedValue
seed_mpfr_const_pi (SeedContext ctx,
                    SeedObject function,
                    SeedObject this_object,
                    gsize argument_count,
                    const SeedValue args[],
                    SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop;
    gint ret;

    CHECK_ARG_COUNT("mpfr.pi", 1);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[0], exception);

    ret = mpfr_const_pi(rop, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

static SeedValue
seed_mpfr_const_euler (SeedContext ctx,
                       SeedObject function,
                       SeedObject this_object,
                       gsize argument_count,
                       const SeedValue args[],
                       SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop;
    gint ret;

    CHECK_ARG_COUNT("mpfr.euler", 1);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[0], exception);

    ret = mpfr_const_euler(rop, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

static SeedValue
seed_mpfr_const_catalan (SeedContext ctx,
                         SeedObject function,
                         SeedObject this_object,
                         gsize argument_count,
                         const SeedValue args[],
                         SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop;
    gint ret;

    CHECK_ARG_COUNT("mpfr.catalan", 1);

    rop = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[0], exception);

    ret = mpfr_const_catalan(rop, rnd);

    return seed_value_from_int(ctx, ret, exception);
}

static SeedValue
seed_mpfr_get_exp (SeedContext ctx,
                   SeedObject this_object,
                   SeedString property_name,
                   SeedException *exception)
{
    mpfr_ptr ptr = seed_object_get_private(this_object);
    return seed_value_from_mp_exp_t(ctx, mpfr_get_exp(ptr), exception);
}

static gboolean
seed_mpfr_set_exp (SeedContext ctx,
                   SeedObject this_object,
                   SeedString property_name,
                   SeedValue value,
                   SeedException *exception)
{
    mpfr_ptr ptr = seed_object_get_private(this_object);
    mpfr_set_exp(ptr, seed_value_to_mp_exp_t(ctx, value, exception));
    return TRUE;
}

static SeedValue
seed_mpfr_get_prec (SeedContext ctx,
                    SeedObject this_object,
                    SeedString property_name,
                    SeedException *exception)
{
    mpfr_ptr ptr = seed_object_get_private(this_object);
    return seed_value_from_mpfr_prec_t(ctx, mpfr_get_prec(ptr), exception);
}

static gboolean
seed_mpfr_set_prec (SeedContext ctx,
                    SeedObject this_object,
                    SeedString property_name,
                    SeedValue value,
                    SeedException *exception)
{
    mpfr_ptr ptr = seed_object_get_private(this_object);
    mpfr_set_prec(ptr, seed_value_to_mpfr_prec_t(ctx, value, exception));
    return TRUE;
}

static void
seed_mpfr_finalize (SeedObject obj)
{
    mpfr_ptr ptr = seed_object_get_private(obj);
    if ( ptr )
    {
        mpfr_clear(ptr);
        g_free( ptr );
    }
}

static SeedValue
seed_mpfr_set (SeedContext ctx,
               SeedObject function,
               SeedObject this_object,
               gsize argument_count,
               const SeedValue args[],
               SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op;
    gdouble dop;
    SeedObject obj;
    gint ret;
    gchar* str;
    seed_mpfr_t argt;

    CHECK_ARG_COUNT("mpfr.set", 2);

    rop = seed_object_get_private(this_object);
    argt = seed_mpfr_arg_type(ctx, args[0], exception);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[1], exception);

    switch ( argt )
    {
        case SEED_MPFR_MPFR:
            op = seed_object_get_private(args[0]);
            ret = mpfr_set(rop, op, rnd);
            break;
        case SEED_MPFR_DOUBLE:
            dop = seed_value_to_double(ctx, args[0], exception);
            ret = mpfr_set_d(rop, dop, rnd);
            break;
        case SEED_MPFR_STRING:
            str = seed_value_to_string(ctx, args[0], exception);
            ret = mpfr_set_str(rop, str, 10, rnd);
            g_free( str );
            break;
        default:
            TYPE_EXCEPTION("mpfr.set", "mpfr_t, double or string");
    }

    return seed_value_from_int(ctx, ret, exception);
}

/* init and set functions, using default precision, or optionally specifying it */
SeedObject
seed_mpfr_construct_with_set(SeedContext ctx,
                             SeedObject constructor,
                             gsize argument_count,
                             const SeedValue args[],
                             SeedException* exception)
{
    mpfr_prec_t prec;
    mpfr_rnd_t rnd;
    mpfr_ptr newmp, op;
    gdouble dbl;
    gchar* str;
    SeedObject obj;
    seed_mpfr_t argt;

    /* TODO: Precision range check */
    switch ( argument_count )
    {
        case 2:
            prec = mpfr_get_default_prec();
            break;
        case 3:
            if ( seed_value_is_number(ctx, args[1]) )
            {
                prec = seed_value_to_mpfr_prec_t(ctx, args[1], exception);
            }
            else
            {
                TYPE_EXCEPTION("mpfr constructor with set", "mpfr_prec_t");
            }
            break;
        default:
            seed_make_exception (ctx, exception, "ArgumentError",
                                 "mpfr_t constructor.set expected 2 or 3 arguments got %Zd", argument_count);
            return seed_make_undefined (ctx);
    }

    /* last argument is always rnd */
    if ( seed_value_is_number(ctx, args[argument_count - 1]) )
        rnd = seed_value_to_mpfr_rnd_t(ctx, args[argument_count - 1], exception);
    else
    {
        TYPE_EXCEPTION("mpfr constructor", "mpfr_rnd_t");
    }

    newmp = (mpfr_ptr) g_malloc(sizeof(mpfr_t));
    mpfr_init2(newmp, prec);

    argt = seed_mpfr_arg_type(ctx, args[0], exception);

    switch ( argt )
    {
        case SEED_MPFR_MPFR:
            obj = seed_value_to_object(ctx, args[0], exception);
            op = seed_object_get_private(obj);
            mpfr_set(newmp, op, rnd);
            break;
        case SEED_MPFR_DOUBLE:
            dbl = seed_value_to_double(ctx, args[0], exception);
            mpfr_set_d(newmp, dbl, rnd);
            break;
        case SEED_MPFR_STRING:
            /* TODO: Assuming base 10 is bad */
            str = seed_value_to_string(ctx, args[0], exception);
            mpfr_set_str(newmp, str, 10, rnd);
            break;
        default:
            mpfr_clear( newmp );
            g_free( newmp );
            TYPE_EXCEPTION("mpfr_constructor", "mpfr_t, double or string");
    }

    return seed_make_object(ctx, mpfr_class, newmp);
}

/* init, optionally specifying precision */
SeedObject
seed_mpfr_construct(SeedContext ctx,
                    SeedObject constructor,
                    gsize argument_count,
                    const SeedValue args[],
                    SeedException* exception)
{
    mpfr_prec_t prec;
    mpfr_ptr newmp = (mpfr_ptr) g_malloc(sizeof(mpfr_t));

    switch (argument_count)
    {
        case 0:
            mpfr_init(newmp);           /* use default precision */
            break;
        case 1:
            if ( seed_value_is_number(ctx, args[0]) )
            {
                prec = seed_value_to_mpfr_prec_t(ctx, args[0], exception);
                mpfr_init2(newmp, prec);
            }
            else
            {
                g_free( newmp );
                TYPE_EXCEPTION("mpfr constructor", "mpfr_prec_t");
            }
            break;
        default:
            seed_make_exception (ctx, exception, "ArgumentError",
                                 "mpfr_t constructor expected 0 or 1 arguments got %Zd", argument_count);
            return seed_make_undefined (ctx);
    }

    return seed_make_object(ctx, mpfr_class, newmp);
}

seed_static_function mpfr_funcs[] =
{
    {"add", seed_mpfr_add, 0},
    {"sin", seed_mpfr_sin, 0},
    {"cos", seed_mpfr_cos, 0},
    {"tan", seed_mpfr_tan, 0},
    {"asin", seed_mpfr_asin, 0},
    {"acos", seed_mpfr_acos, 0},
    {"atan", seed_mpfr_atan, 0},
    {"csc", seed_mpfr_csc, 0},
    {"sec", seed_mpfr_sec, 0},
    {"cot", seed_mpfr_cot, 0},
    {"log", seed_mpfr_log, 0},
    {"log2", seed_mpfr_log2, 0},
    {"log10", seed_mpfr_log10, 0},
    {"set", seed_mpfr_set, 0},
    {"out_str", seed_mpfr_out_str, 0},
    {"pi", seed_mpfr_const_pi, 0},
    {"euler", seed_mpfr_const_pi, 0},
    {"catalan", seed_mpfr_const_pi, 0},
    {NULL, NULL, 0}
};

seed_static_value mpfr_values[] =
{
    {"prec", seed_mpfr_get_prec, seed_mpfr_set_prec, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
    {"exp", seed_mpfr_get_exp, seed_mpfr_set_exp, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
    {NULL, 0, NULL, 0}
};

SeedObject
seed_module_init(SeedEngine *local_eng)
{
    SeedGlobalContext ctx = local_eng->context;
    SeedObject ctx_constructor_rew;
    seed_class_definition mpfr_def = seed_empty_class;
    SeedObject mpfr_constructor, mpfr_constructor_set;

    ns_ref = seed_make_object(ctx, NULL, NULL);

    mpfr_def.class_name = "mpfr_t";
    mpfr_def.static_functions = mpfr_funcs;
    mpfr_def.finalize = seed_mpfr_finalize;
    mpfr_def.static_values = mpfr_values;

    mpfr_class = seed_create_class(&mpfr_def);

    mpfr_constructor = seed_make_constructor(ctx, mpfr_class, seed_mpfr_construct);
    mpfr_constructor_set = seed_make_constructor(ctx, mpfr_class, seed_mpfr_construct_with_set);

    seed_object_set_property(ctx, ns_ref, "mpfr_t", mpfr_constructor);
    seed_object_set_property(ctx, mpfr_constructor, "set", mpfr_constructor_set);

    /* Setup enums */
    DEFINE_ENUM_MEMBER(ns_ref, GMP_RNDN);
    DEFINE_ENUM_MEMBER(ns_ref, GMP_RNDZ);
    DEFINE_ENUM_MEMBER(ns_ref, GMP_RNDU);
    DEFINE_ENUM_MEMBER(ns_ref, GMP_RNDD);

    DEFINE_ENUM_MEMBER_EXT(ns_ref, "VERSION_MAJOR", MPFR_VERSION_MAJOR);
    DEFINE_ENUM_MEMBER_EXT(ns_ref, "VERSION_MINOR", MPFR_VERSION_MINOR);
    DEFINE_ENUM_MEMBER_EXT(ns_ref, "VERSION_PATCHLEVEL", MPFR_VERSION_PATCHLEVEL);
    seed_object_set_property(ctx, ns_ref, "VERSION_STRING",
                             seed_value_from_string(ctx, MPFR_VERSION_STRING, NULL));

    DEFINE_ENUM_MEMBER_EXT(ns_ref, "PREC_MIN", MPFR_PREC_MIN);
    DEFINE_ENUM_MEMBER_EXT(ns_ref, "PREC_MAX", MPFR_PREC_MAX);

    DEFINE_ENUM_MEMBER_EXT(ns_ref, "EMAX_DEFAULT", MPFR_EMAX_DEFAULT);
    DEFINE_ENUM_MEMBER_EXT(ns_ref, "EMIN_DEFAULT", MPFR_EMIN_DEFAULT);

    DEFINE_ENUM_MEMBER_EXT(ns_ref, "NAN_KIND", MPFR_NAN_KIND);
    DEFINE_ENUM_MEMBER_EXT(ns_ref, "INF_KIND", MPFR_INF_KIND);
    DEFINE_ENUM_MEMBER_EXT(ns_ref, "ZERO_KIND", MPFR_ZERO_KIND);
    DEFINE_ENUM_MEMBER_EXT(ns_ref, "REGULAR_KIND", MPFR_REGULAR_KIND);

    return ns_ref;
}

