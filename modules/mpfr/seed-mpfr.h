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

DEF_SEED_MPFR_FUNC(seed_mpfr_neg);
DEF_SEED_MPFR_FUNC(seed_mpfr_abs);
DEF_SEED_MPFR_FUNC(seed_mpfr_dim);

DEF_SEED_MPFR_FUNC(seed_mpfr_rint);
DEF_SEED_MPFR_FUNC(seed_mpfr_ceil);
DEF_SEED_MPFR_FUNC(seed_mpfr_floor);
DEF_SEED_MPFR_FUNC(seed_mpfr_round);
DEF_SEED_MPFR_FUNC(seed_mpfr_trunc);
DEF_SEED_MPFR_FUNC(seed_mpfr_frac);

DEF_SEED_MPFR_FUNC(seed_mpfr_nexttoward);
DEF_SEED_MPFR_FUNC(seed_mpfr_nextabove);
DEF_SEED_MPFR_FUNC(seed_mpfr_nextbelow);

DEF_SEED_MPFR_FUNC(seed_mpfr_prec_round);
DEF_SEED_MPFR_FUNC(seed_mpfr_signbit);
DEF_SEED_MPFR_FUNC(seed_mpfr_setsign);
DEF_SEED_MPFR_FUNC(seed_mpfr_copysign);

DEF_SEED_MPFR_FUNC(seed_mpfr_min);
DEF_SEED_MPFR_FUNC(seed_mpfr_max);

DEF_SEED_MPFR_FUNC(seed_mpfr_fmod);
DEF_SEED_MPFR_FUNC(seed_mpfr_remainder);
DEF_SEED_MPFR_FUNC(seed_mpfr_integer_p);

DEF_SEED_MPFR_FUNC(seed_mpfr_rint_ceil);
DEF_SEED_MPFR_FUNC(seed_mpfr_rint_floor);
DEF_SEED_MPFR_FUNC(seed_mpfr_rint_round);
DEF_SEED_MPFR_FUNC(seed_mpfr_rint_trunc);

DEF_SEED_MPFR_FUNC(seed_mpfr_add);
DEF_SEED_MPFR_FUNC(seed_mpfr_sub);
DEF_SEED_MPFR_FUNC(seed_mpfr_mul);
DEF_SEED_MPFR_FUNC(seed_mpfr_div);

DEF_SEED_MPFR_FUNC(seed_mpfr_sin);
DEF_SEED_MPFR_FUNC(seed_mpfr_cos);
DEF_SEED_MPFR_FUNC(seed_mpfr_tan);

DEF_SEED_MPFR_FUNC(seed_mpfr_csc);
DEF_SEED_MPFR_FUNC(seed_mpfr_sec);
DEF_SEED_MPFR_FUNC(seed_mpfr_cot);

DEF_SEED_MPFR_FUNC(seed_mpfr_asin);
DEF_SEED_MPFR_FUNC(seed_mpfr_acos);
DEF_SEED_MPFR_FUNC(seed_mpfr_atan);

DEF_SEED_MPFR_FUNC(seed_mpfr_log);
DEF_SEED_MPFR_FUNC(seed_mpfr_log2);
DEF_SEED_MPFR_FUNC(seed_mpfr_log10);

DEF_SEED_MPFR_FUNC(seed_mpfr_sinh);
DEF_SEED_MPFR_FUNC(seed_mpfr_cosh);
DEF_SEED_MPFR_FUNC(seed_mpfr_tanh);

DEF_SEED_MPFR_FUNC(seed_mpfr_csch);
DEF_SEED_MPFR_FUNC(seed_mpfr_sech);
DEF_SEED_MPFR_FUNC(seed_mpfr_coth);

DEF_SEED_MPFR_FUNC(seed_mpfr_asinh);
DEF_SEED_MPFR_FUNC(seed_mpfr_acosh);
DEF_SEED_MPFR_FUNC(seed_mpfr_atanh);

DEF_SEED_MPFR_FUNC(seed_mpfr_sqrt);
DEF_SEED_MPFR_FUNC(seed_mpfr_sqr);
DEF_SEED_MPFR_FUNC(seed_mpfr_root);
DEF_SEED_MPFR_FUNC(seed_mpfr_rec_sqrt);
DEF_SEED_MPFR_FUNC(seed_mpfr_cbrt);

DEF_SEED_MPFR_FUNC(seed_mpfr_cmp);
DEF_SEED_MPFR_FUNC(seed_mpfr_cmpabs);

DEF_SEED_MPFR_FUNC(seed_mpfr_nan_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_inf_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_number_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_zero_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_sgn);

DEF_SEED_MPFR_FUNC(seed_mpfr_greater_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_greaterequal_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_less_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_lessequal_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_lessgreater_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_equal_p);
DEF_SEED_MPFR_FUNC(seed_mpfr_unordered_p);

DEF_SEED_MPFR_FUNC(seed_mpfr_log1p);
DEF_SEED_MPFR_FUNC(seed_mpfr_expm1);
DEF_SEED_MPFR_FUNC(seed_mpfr_li2);
DEF_SEED_MPFR_FUNC(seed_mpfr_gamma);
DEF_SEED_MPFR_FUNC(seed_mpfr_lngamma);
DEF_SEED_MPFR_FUNC(seed_mpfr_zeta);
DEF_SEED_MPFR_FUNC(seed_mpfr_erf);
DEF_SEED_MPFR_FUNC(seed_mpfr_erfc);

DEF_SEED_MPFR_FUNC(seed_mpfr_j0);
DEF_SEED_MPFR_FUNC(seed_mpfr_j1);
DEF_SEED_MPFR_FUNC(seed_mpfr_jn);
DEF_SEED_MPFR_FUNC(seed_mpfr_y0);
DEF_SEED_MPFR_FUNC(seed_mpfr_y1);
DEF_SEED_MPFR_FUNC(seed_mpfr_yn);

DEF_SEED_MPFR_FUNC(seed_mpfr_fma);
DEF_SEED_MPFR_FUNC(seed_mpfr_fms);
DEF_SEED_MPFR_FUNC(seed_mpfr_agm);
DEF_SEED_MPFR_FUNC(seed_mpfr_hypot);

DEF_SEED_MPFR_FUNC(seed_mpfr_free_cache);
DEF_SEED_MPFR_FUNC(seed_mpfr_clear_flags);


#endif      /* _SEED_MFPR_H_ */

