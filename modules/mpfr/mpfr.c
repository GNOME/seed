#include <seed.h>
#include <mpfr.h>

SeedObject ns_ref;
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

    #if _MPFR_PREC_FORMAT == 1
    prec = seed_value_to_ushort(ctx, args[1], exception);
    #elif _MPFR_PREC_FORMAT == 2
    prec = seed_value_to_uint(ctx, args[1], exception);
    #elif _MPFR_PREC_FORMAT == 3
    prec = seed_value_to_ulong(ctx, args[1], exception);
    #else
    #error "Invalid MPFR Prec format"
    #endif

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

SeedObject
seed_module_init(SeedEngine *local_eng)
{
    SeedGlobalContext ctx = local_eng->context;
    ns_ref = seed_make_object (ctx, NULL, NULL);
    seed_value_protect (ctx, ns_ref);

    seed_create_function(ctx, "init2",
                         (SeedFunctionCallback) seed_mpfr_init2,
                         ns_ref);
    seed_create_function(ctx, "add",
                         (SeedFunctionCallback) seed_mpfr_add,
                         ns_ref);

    return ns_ref;
}

