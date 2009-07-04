#include "../../libseed/seed.h"

#include <stdio.h>
#include <mpfr.h>

SeedObject ns_ref;
SeedClass mpfr_class;

#if 0 /* TODO: Make this work */
/* kind of stupid hack */
#if MPFR_PREC_MAX == G_MAXLONG
    #define seed_value_to_mpfr_prec_t(a, b, c) seed_value_to_ulong(a, b, c)
    #define seed_value_from_mpfr_prec_t(a, b, c) seed_value_from_ulong(a, b, c)
#elif MPFR_PREC_MAX == G_MAXUSHORT
    #define seed_value_to_mpfr_prec_t(a, b, c) seed_value_to_ushort(a, b, c)
    #define seed_value_from_mpfr_prec_t(a, b, c) seed_value_from_ushort(a, b, c)
#elif MPFR_PREC_MAX == G_MAXINT
    #define seed_value_to_mpfr_prec_t(a, b, c) seed_value_to_int(a, b, c)
    #define seed_value_from_mpfr_prec_t(a, b, c) seed_value_from_int(a, b, c)
#elif MPFR_PREC_MAX == G_MAXUINT64
    #define seed_value_to_mpfr_prec_t(a, b, c) seed_value_to_uint64(a, b, c)
    #define seed_value_from_mpfr_prec_t(a, b, c) seed_value_from_uint64(a, b, c)
#else
    #error "Wrong mpfr_prec_t size somehow?"
#endif
#endif

#define seed_value_to_mpfr_prec_t(a, b, c) seed_value_to_uint64(a, b, c)
#define seed_value_from_mpfr_prec_t(a, b, c) seed_value_from_uint64(a, b, c)

/* TODO: Right size for this */
#define seed_value_to_mpfr_rnd_t(a, b, c) seed_value_to_char(a, b, c)
#define seed_value_from_mpfr_rnd_t(a, b, c) seed_value_from_char(a, b, c)

/* For now at least ignoring the ability to use gmp types since there is no gmp module */

SeedEngine * eng;

typedef enum _seed_mpfr_t
{
    SEED_MPFR_UNKNOWN = 0,
    SEED_MPFR_MPFR = 1 << 1,
    SEED_MPFR_DOUBLE = 1 << 2,
    SEED_MPFR_STRING = 1 << 3,
} seed_mpfr_t;

static inline seed_mpfr_t
seed_mpfr_arg_type(SeedContext ctx, SeedValue arg, SeedException exept)
{
    if ( seed_value_is_object_of_class(ctx, arg, mpfr_class) )
        return SEED_MPFR_MPFR;
    else if ( seed_value_is_number(ctx, arg) )
        return SEED_MPFR_DOUBLE;
    else
        return SEED_MPFR_UNKNOWN;
}

static SeedValue
seed_mpfr_out_str (SeedContext ctx,
                   SeedObject function,
                   SeedObject this_object,
                   gsize arg_count,
                   const SeedValue args[],
                   SeedException * exception)
{
    size_t n;
    FILE* stream;
    gint base;
    mpfr_rnd_t rnd;
    mpfr_ptr op;

    if ( arg_count != 4 )
    {
        seed_make_exception (ctx, exception, "ArgumentError",
                             "mpfr.out_str expected 4 arguments, got %zd",
                             arg_count);
        return seed_make_null (ctx);
    }

    stream = (FILE*) seed_pointer_get_pointer(ctx, args[0]);
    base = seed_value_to_int(ctx, args[1], exception);
    n = seed_value_to_uint(ctx, args[2], exception);
    op = seed_object_get_private(this_object);
    rnd = seed_value_to_mpfr_rnd_t(ctx, args[3], exception);

    return seed_value_from_uint(ctx,
                                mpfr_out_str(stream, base, n, op, rnd),
                                exception);
}

/* This is a bit disgusting. Oh well. */
static SeedValue
seed_mpfr_add (SeedContext ctx,
               SeedObject function,
               SeedObject this_object,
               gsize arg_count,
               const SeedValue args[],
               SeedException * exception)
{
    mpfr_rnd_t rnd;
    mpfr_ptr rop, op1, op2;
    gdouble dop1, dop2;
    SeedObject obj;
    gint ret;
    seed_mpfr_t argt1, argt2;
    /* only want 1 double argument. alternatively, could accept 2,
       add those, and set from the result*/

    if ( arg_count != 3 )
    {
        seed_make_exception (ctx, exception, "ArgumentError",
                             "mpfr.add expected 3 arguments, got %zd",
                             arg_count);
        return seed_make_null (ctx);
    }

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
        seed_make_exception (ctx, exception, "TypeError",
                             "mpfr add received unexpected type");
        return seed_make_null(ctx);
    }

    return seed_value_from_int(ctx, ret, exception);
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
               gsize arg_count,
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

    if ( arg_count != 2 )
    {
        seed_make_exception (ctx, exception, "ArgumentError",
                             "mpfr.set expected 2 arguments, got %zd",
                             arg_count);
        return seed_make_null (ctx);
    }

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
            seed_make_exception (ctx, exception, "TypeError",
                                 "mpfr set received unexpected type");
            return seed_make_null(ctx);
    }

    return seed_value_from_int(ctx, ret, exception);
}

/* init and set functions, using default precision, or optionally specifying it */
SeedObject
seed_mpfr_construct_with_set(SeedContext ctx,
                             SeedObject constructor,
                             size_t arg_count,
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
    switch ( arg_count )
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
                seed_make_exception (ctx, exception, "TypeError",
                                     "mpfr constructor with set expected number as precision");
                return seed_make_null(ctx);
            }
            break;
        default:
            seed_make_exception (ctx, exception, "ArgumentError",
                                 "mpfr constructor with init expected 2 or 3 arguments, got %zd",
                                 arg_count);
            return seed_make_null (ctx);
    }

    /* last argument is always rnd */
    if ( seed_value_is_number(ctx, args[arg_count - 1]) )
        rnd = seed_value_to_mpfr_rnd_t(ctx, args[arg_count - 1], exception);
    else
    {
        seed_make_exception (ctx, exception, "TypeError",
                             "mpfr constructor expected number as round mode");
        return seed_make_null(ctx);
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
            seed_make_exception (ctx, exception, "TypeError",
                                 "mpfr constructor with set expected initializer as mpfr, number, or string");
            mpfr_clear( newmp );
            g_free( newmp );
            return seed_make_null(ctx);
    }

    return seed_make_object(ctx, mpfr_class, newmp);
}

/* init, optionally specifying precision */
SeedObject
seed_mpfr_construct(SeedContext ctx,
                    SeedObject constructor,
                    size_t arg_count,
                    const SeedValue args[],
                    SeedException* exception)
{
    mpfr_prec_t prec;
    mpfr_ptr newmp = (mpfr_ptr) g_malloc(sizeof(mpfr_t));

    switch (arg_count)
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
                seed_make_exception (ctx, exception, "TypeError",
                                     "mpfr constructor expected number as precision");
                g_free( newmp );
                return seed_make_null(ctx);
            }
            break;
        default:
            seed_make_exception (ctx, exception, "ArgumentError",
                                 "mpfr constructor expected 0 or 1 argument, got %zd", arg_count);
            break;
    }

    return seed_make_object(ctx, mpfr_class, newmp);
}

seed_static_function mpfr_funcs[] =
{
    {"add", seed_mpfr_add, 0},
    {"set", seed_mpfr_set, 0},
    {"out_str", seed_mpfr_out_str, 0},
    {NULL, NULL, 0}
};

seed_static_value mpfr_values[] =
{
    {"prec", seed_mpfr_get_prec, seed_mpfr_set_prec, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
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

    mpfr_def.class_name = "mpfr";
    mpfr_def.static_functions = mpfr_funcs;
    mpfr_def.finalize = seed_mpfr_finalize;
    mpfr_def.static_values = mpfr_values;

    mpfr_class = seed_create_class(&mpfr_def);

    mpfr_constructor = seed_make_constructor(ctx, mpfr_class, seed_mpfr_construct);
    mpfr_constructor_set = seed_make_constructor(ctx, mpfr_class, seed_mpfr_construct_with_set);

    seed_object_set_property(ctx, ns_ref, "mpfr", mpfr_constructor);
    seed_object_set_property(ctx, mpfr_constructor, "set", mpfr_constructor_set);

    return ns_ref;
}

