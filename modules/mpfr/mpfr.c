#include "../../libseed/seed.h"
#include <mpfr.h>

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
seed_mpfr_init2 (SeedContext ctx,
                 SeedObject function,
                 SeedObject this_object,
                 gsize arg_count,
                 const SeedValue args[],
                 SeedException * exception)
{
    mpfr_prec_t prec;
    mpfr_ptr ptr;

    if ( arg_count != 2 )
    {
        seed_make_exception (ctx, exception, "ArgumentError",
                             "mpfr_init2 expected 2 arguments, got %zd",
                             arg_count);
        return seed_make_null (ctx);
    }

    ptr = seed_pointer_get_pointer(ctx, args[0]);
    prec = seed_value_to_mpfr_prec_t(ctx, args[1], exception);

    mpfr_init2(ptr, prec);

    return seed_make_null(ctx);
}


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
    mpfr_ptr ptr = seed_pointer_get_pointer(ctx, this_object);
    return seed_value_from_mpfr_prec_t(ctx, mpfr_get_prec(ptr), exception);
}

static gboolean
seed_mpfr_set_prec (SeedContext ctx,
                    SeedObject this_object,
                    SeedString property_name,
                    SeedValue value,
                    SeedException *exception)
{
    mpfr_ptr ptr = seed_pointer_get_pointer(ctx, this_object);
    mpfr_set_prec(ptr, seed_value_to_mpfr_prec_t(ctx, value, exception));
    return TRUE;
}

static void
seed_mpfr_finalize (SeedObject obj)
{
    mpfr_ptr ptr = seed_pointer_get_pointer(eng->context, obj);
    if ( ptr )
        mpfr_clear(ptr);
}

seed_static_function mpfr_funcs[] =
{
    {"init2", seed_mpfr_init2, 0},
    {"add", seed_mpfr_add, 0},
    {NULL, NULL, 0}
};

seed_static_value mpfr_values[] =
{
    {"precision", seed_mpfr_get_prec, seed_mpfr_set_prec, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
    {NULL, 0, NULL, 0}
};

SeedObject
seed_module_init(SeedEngine *local_eng)
{
    SeedGlobalContext ctx = local_eng->context;
    SeedObject ctx_constructor_rew;
    SeedObject ns_ref;
    seed_class_definition mpfr_def = seed_empty_class;

    ns_ref = seed_make_object(ctx, NULL, NULL);
    seed_value_protect (ctx, ns_ref);

    mpfr_def.class_name = "mpfr";
    mpfr_def.static_functions = mpfr_funcs;
    mpfr_def.finalize = seed_mpfr_finalize;
    mpfr_def.static_values = mpfr_values;


    return ns_ref;
}

