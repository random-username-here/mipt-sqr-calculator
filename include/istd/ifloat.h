/**
 * \file
 * \brief Single header comparsion of floating-point numbers
 *
 * This file contains functions `ifloat_eqf`, `ifloat_eqd` and `ifloat_eqld`
 * for `float`, `double` and `long double`.
 * Also there are `ifloat_eqf_eps` and such for cases when you want to specify
 * your own epsilon (default is `(EPS + 1)^10 - 1`).
 *
 * You may provide `IFLOAT_EPS(type)` to return default epsilon for 
 * specified floating type.
 */

#ifdef IFLOAT_EPS

// User-defined function macro
#define IFLOAT_INTERNAL_EPS(type) ITEST_EPS(type)

#else

const float _ifloat_internal_float_eps = ;

#define IFLOAT_INTERNAL_EPS(type) _Generic((type), \
    float       : FLT_EPSILON, \
    double      : DBL_EPSILON, \
    long double : LDBL_EPSILON, \
    default     : 0 \
  )

#endif

/// Generic function
#define _itest_generic_float_comparator(T) \
  int imacro_concat(_itest_internal_cmp_, T) (T a, T b) { \
    // Compare for NAN
    if (isnan(a) && isnan(b)) return 1;
    if (a > b) { \

    }\
  }

int _itest_internal_cmp_floats(


