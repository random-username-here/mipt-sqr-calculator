/**
 * \file
 * \brief Primitive single-header-file testing library.
 *
 * It does not need implementation, there are only macros.
 *
 * Syntax for this is:
 * ```c
 * imini_test_case("Test of abs()")
 *   imini_test_assert(something, "Something shall be true");
 *   imini_test_assert_equal(get_answer(), 42, "Answer must be an actual answer");
 * imini_test_case_end
 * ```
 */

#ifndef ISTD_IMINI_TEST
#define ISTD_IMINI_TEST

#include "istd/imacro.h"
#include <stdio.h>
#include <string.h>

/// \defgroup imini_test_colors ASCII escape codes for use inside `imini-test.h`
/// \addtogroup imini_test_colors
/// \{
#define _IMINI_TEST_E_GRAY  "\x1b[90m"
#define _IMINI_TEST_E_RED   "\x1b[91m"
#define _IMINI_TEST_E_GREEN "\x1b[92m"
#define _IMINI_TEST_E_BOLD  "\x1b[1m"
#define _IMINI_TEST_E_RESET "\x1b[0m"
/// \}

//------- Function part ----------------------------------//

/// \internal
/// `constructor` attribute for function.
/// \todo Find etter supported variant of this
#define _imini_test_ctor __attribute__((constructor))

/// Begin test case, printing nice header.
/// End this case with `imini_test_case_end`.
/// \param name - Name of the test case, a string.
#define imini_test_case(name) \
  _imini_test_ctor static void imacro_concat(_imini_test_, __COUNTER__) () { \
    const char* _imini_test__name = (name); \
    const size_t _imini_test__name_len = strlen(_imini_test__name); \
    printf("\n" \
        _IMINI_TEST_E_GRAY "---- " _IMINI_TEST_E_RESET \
        _IMINI_TEST_E_BOLD "%s" _IMINI_TEST_E_RESET \
        _IMINI_TEST_E_GRAY " ", _imini_test__name); \
    for (size_t i = 7 + _imini_test__name_len; i < 80; ++i) \
      printf("-"); \
    printf(_IMINI_TEST_E_RESET "\n\n");

/// End of test case. Prints "All test passed" message if
/// everything is okay.
#define imini_test_case_end \
  printf("\n" _IMINI_TEST_E_GREEN "All tests passed" _IMINI_TEST_E_RESET "\n"); }

/// \internal
/// Write given value to stdout.
/// \param value - Value to print. Can be anything.
#define _imini_test_print(value) \
  _Generic((value), \
    int: printf("%d", (value)), \
    double: printf("%lf", (value)), \
    default: printf("<value of unknown type>") \
  )

//------- Assert part ------------------------------------//

// Diffrent status messages printed by tester.
// Must be the same length to print properly.
// (you can pad them with spaces to achive that)
#define _IMINI_TEST_PROGRESS_MSG _IMINI_TEST_E_GRAY  "...." _IMINI_TEST_E_RESET
#define _IMINI_TEST_FAIL_MSG     _IMINI_TEST_E_RED   "FAIL" _IMINI_TEST_E_RESET
#define _IMINI_TEST_OK_MSG       _IMINI_TEST_E_GREEN "PASS" _IMINI_TEST_E_RESET

/// \internal
/// Check what condition is true, if not print something with printer.
/// \param condition - Boolean condition to check
/// \param label     - Label for this check
/// \param printer   - Block of code to be called when condition fails,
///                    like `{ printf("We are doomed!\n"); }`
#define _imini_test_assert_base(condition, label, printer) { \
    const char* _imini_test__label = (label); \
    printf(" %s %s", _IMINI_TEST_PROGRESS_MSG, _imini_test__label); \
    if ((condition)) { \
      printf("\r %s\n", _IMINI_TEST_OK_MSG); \
    } else { \
      printf("\r %s\n", _IMINI_TEST_FAIL_MSG); \
      printf("\n"); \
      printer; \
      printf("\n"); /* Some space after end of test section */ \
      return; \
    } \
  }

/// Assert what condition is `true`
/// \param condition - Condition to check
/// \param label - Label of this assert, like `"The value must be positive"`
#define imini_test_assert(condition, label) \
    _imini_test_assert_base(condition, label,\
      { printf("   " _IMINI_TEST_E_RED "%s" _IMINI_TEST_E_RESET " is not true\n", #condition); })

/// \internal
/// Compare `a` and `b` using given comparator and fail this test if they are not equal.
/// \param condition - Comparator function macro with signature `bool COMPARE(a, b)`
/// \param a     - First value
/// \param b     - Second value
/// \param label - Label of the assert
/// \param VA_ARGS - Extra params to pass to comparator
#define _imini_test_assert_equal_base(condition, a, b, label, ...) { \
    typeof(a) _imini_test__a = (a); \
    typeof(b) _imini_test__b = (b); \
    _imini_test_assert_base( \
      condition(_imini_test__a, _imini_test__b __VA_OPT__(,) __VA_ARGS__), \
      label, \
      { \
        printf("   " _IMINI_TEST_E_RED "%s" _IMINI_TEST_E_RESET " (a) = ", #a); \
        _imini_test_print(_imini_test__a); \
        printf("\n   " _IMINI_TEST_E_RED "%s" _IMINI_TEST_E_RESET " (b) = ", #b); \
        _imini_test_print(_imini_test__b); \
        printf("\n"); \
      } \
    ) \
  }

/// \internal
/// Comparator used in \ref imini_test_assert_equal
#define _imini_test_equal_sign(a, b) (a == b)

/// Check equality of `a` and `b` and fail this test if they are not equal.
/// \param a     - First value
/// \param b     - Second value
/// \param label - Label of the assert
#define imini_test_assert_equal(a, b, label) \
  _imini_test_assert_equal_base(_imini_test_equal_sign, a, b, label)

/// \internal
/// Comparator used in \ref imini_test_assert_somewhat_equal
#define _imini_test_equality_eps(a, b, eps) (a < b * (1 + eps) && a > b * (1 - eps))

/// Check if floats `a` and `b` are equal with given relative `epsilon`
/// \param a     - First value
/// \param b     - Second value
/// \param eps   - Epsilon.
/// \param label - Label of the assert
#define imini_test_assert_somewhat_equal(a, b, eps, label) { \
    typeof(eps) _imini_test__eps = (eps); \
    _imini_test_assert_equal_base(_imini_test_equality_eps, a, b, label, _imini_test__eps);\
  }


#endif
