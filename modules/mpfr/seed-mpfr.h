#ifndef _SEED_MPFR_H_
#define _SEED_MPFR_H_

#include <seed-module.h>
#include <seed.h>

#define TYPE_EXCEPTION(name, wanted) \
    seed_make_exception(ctx, exception, "TypeError", name " expected " wanted ); \
    return seed_make_undefined(ctx);

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

#define seed_value_from_mp_exp_t(a, b, c) seed_value_from_ulong(a, b, c)
#define seed_value_to_mp_exp_t(a, b, c) seed_value_to_ulong(a, b, c)


#define DEF_SEED_MPFR_FUNC(name) SeedValue name( SeedContext,SeedObject,\
SeedObject, gsize, const SeedValue[], SeedException*)

SeedObject ns_ref;
SeedClass mpfr_class;

typedef enum _seed_mpfr_t
{
    SEED_MPFR_UNKNOWN = 0,
    SEED_MPFR_MPFR = 1 << 1,
    SEED_MPFR_DOUBLE = 1 << 2,
    SEED_MPFR_STRING = 1 << 3,
} seed_mpfr_t;

inline seed_mpfr_t seed_mpfr_arg_type(SeedContext, SeedValue, SeedException);

DEF_SEED_MPFR_FUNC(seed_mpfr_add);
DEF_SEED_MPFR_FUNC(seed_mpfr_sin);
DEF_SEED_MPFR_FUNC(seed_mpfr_cos);
DEF_SEED_MPFR_FUNC(seed_mpfr_tan);

DEF_SEED_MPFR_FUNC(seed_mpfr_csc);
DEF_SEED_MPFR_FUNC(seed_mpfr_sec);
DEF_SEED_MPFR_FUNC(seed_mpfr_cot);

DEF_SEED_MPFR_FUNC(seed_mpfr_asin);
DEF_SEED_MPFR_FUNC(seed_mpfr_acos);
DEF_SEED_MPFR_FUNC(seed_mpfr_atan);

#endif      /* _SEED_MFPR_H_ */

