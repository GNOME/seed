#ifndef _SEED_MPFR_H_
#define _SEED_MPFR_H_

#include <seed-module.h>
#include <seed.h>

#define DEF_SEED_MPFR_FUNC(name) SeedValue name( SeedContext,SeedObject,\
SeedObject, gsize, const SeedValue[], SeedException*)

DEF_SEED_MPFR_FUNC(seed_mpfr_add);
DEF_SEED_MPFR_FUNC(seed_mpfr_sin);

#endif      /* _SEED_MFPR_H_ */

