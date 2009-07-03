#include "../../libseed/seed.h"
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

SeedEngine * eng;

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

    if ( arg_count != 4 )
    {
        seed_make_exception (ctx, exception, "ArgumentError",
                             "mpfr.add expected 4 arguments, got %zd",
                             arg_count);
        return seed_make_null (ctx);
    }

    /* TODO: Peter says that won't work */
    rop = seed_pointer_get_pointer(ctx, args[0]);
    op1 = seed_pointer_get_pointer(ctx, args[1]);
    op2 = seed_pointer_get_pointer(ctx, args[2]);
    rnd = seed_value_to_char(ctx, args[3], exception);

    return seed_value_from_int(ctx, mpfr_add(rop, op1, op2, rnd), exception);
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

    /* TODO: Precision range check */
    if ( arg_count == 2 )
        prec = mpfr_get_default_prec();
    else if ( arg_count == 3 )
    {
        if ( seed_value_is_number(ctx, args[1]) )
            prec = seed_value_to_int(ctx, args[1], exception);
        else
        {
            seed_make_exception (ctx, exception, "TypeError",
                                 "mpfr constructor with set expected number as precision");
            return seed_make_null(ctx);
        }
    }
    else
    {
        seed_make_exception (ctx, exception, "ArgumentError",
                             "mpfr constructor with init expected 2 or 3 arguments, got %zd",
                             arg_count);
        return seed_make_null (ctx);
    }

    /* last argument is always rnd */
    /* TODO: Right size for mpfr_rnd_t */
    if ( seed_value_is_number(ctx, args[arg_count - 1]) )
        rnd = seed_value_to_int(ctx, args[arg_count - 1], exception);
    else
    {
        seed_make_exception (ctx, exception, "TypeError",
                             "mpfr constructor expected number as round mode");
        return seed_make_null(ctx);
    }

    newmp = (mpfr_ptr) g_malloc(sizeof(mpfr_t));

    if ( seed_value_is_object_of_class(ctx, args[0], mpfr_class) )
    {
        obj = seed_value_to_object(ctx, args[0], exception);
        op = seed_object_get_private(obj);
        mpfr_init_set(newmp, op, rnd);
    }
    else if ( seed_value_is_number(ctx, args[0]) )
    {
        dbl = seed_value_to_double(ctx, args[0], exception);
        mpfr_init_set_d(newmp, dbl, rnd);
    }
    else if ( seed_value_is_string(ctx, args[0]) )
    {
        /* TODO: Assuming base 10 is bad */
        str = seed_value_to_string(ctx, args[0], exception);
        mpfr_init_set_str(newmp, str, 10, rnd);
    }
    else
    {
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

