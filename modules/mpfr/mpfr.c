#include <seed.h>
#include <mpfr.h>

SeedObject ns_ref;
SeedEngine * eng;

static void
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
    prec = seed_value_from_ushort(ctx, args[1], exception);
    #elif _MPFR_PREC_FORMAT == 2
    prec = seed_value_from_uint(ctx, args[1], exception);
    #elif _MPFR_PREC_FORMAT == 3
    prec = seed_value_from_ulong(ctx, args[1], exception);
    #else
    #error "Invalid MPFR Prec format"
    #endif

    mpfr_init2(ptr, prec);

    return;
}

SeedObject
seed_module_init(SeedEngine *local_eng)
{
    SeedGlobalContext ctx = local_eng->context;
    ns_ref = seed_make_object (ctx, NULL, NULL);
    seed_value_protect (ctx, namespace_ref);

    seed_create_function(ctx, "init2",
                         (SeedFunctionCallback) seed_mpfr_init2,
                         ns_ref);

    seed_object_set_property(ctx, namespace_ref, "LC_CTYPE",
                             seed_value_from_long(ctx, LC_CTYPE, NULL));


    return ns_ref;
}

