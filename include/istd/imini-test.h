/**
 * \file
 * \brief Primitive single-header-file testing library.
 *
 * Implement this using `ISTD_IMINI_TEST_IMPLEMENTATION` macro before include in one file.
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
#include <setjmp.h>

// ASCII escape codes for use inside `imini-test.h`
#define _IMINI_TEST_E_GRAY  "\x1b[90m"
#define _IMINI_TEST_E_RED   "\x1b[91m"
#define _IMINI_TEST_E_GREEN "\x1b[92m"
#define _IMINI_TEST_E_BOLD  "\x1b[1m"
#define _IMINI_TEST_E_RESET "\x1b[0m"

//------- Function part ----------------------------------//

/// \internal
/// `constructor` attribute for function.
/// \todo Find better supported variant of this
#define _imini_test_ctor __attribute__((constructor))

#define _IMINI_TEST_HEADER_PREFIX "---- "
#define _IMINI_TEST_HEADER_SUFFIX " "
#define _IMINI_TEST_HEADER_WIDTH 80

/// \internal
/// Longjump buffer to jump to "case failed message"
/// Global because you would not want to pass it around.
/// TODO: some flag to check if this is set?
extern jmp_buf _imini_test__jmpbuf_to_fail;

/// Printf some header to split parts of test section apart.
/// \param fmt     - Your usual format string
/// \param VA_ARGS - Params corresponding to that format string
#define imini_test_header(fmt, ...) \
  printf(_IMINI_TEST_E_GRAY "\n## " _IMINI_TEST_E_RESET \
         _IMINI_TEST_E_BOLD fmt "\n\n" _IMINI_TEST_E_RESET __VA_OPT__(,) __VA_ARGS__)

/// Begin test case, printing nice header.
/// End this case with `imini_test_case_end`.
/// \param name - Name of the test case, a string.
#define imini_test_case(name) \
  _imini_test_ctor static void imacro_concat(_imini_test_, __COUNTER__) () { \
    const char* _imini_test__name = (name); \
    const size_t _imini_test__name_len = strlen(_imini_test__name); \
    printf("\n" \
        _IMINI_TEST_E_GRAY _IMINI_TEST_HEADER_PREFIX _IMINI_TEST_E_RESET \
        _IMINI_TEST_E_BOLD "%s" _IMINI_TEST_E_RESET \
        _IMINI_TEST_E_GRAY _IMINI_TEST_HEADER_SUFFIX, _imini_test__name); \
    for (size_t i = sizeof(_IMINI_TEST_HEADER_PREFIX) \
                  + sizeof(_IMINI_TEST_HEADER_SUFFIX) \
                  + _imini_test__name_len; i < _IMINI_TEST_HEADER_WIDTH; ++i) \
      printf("-"); \
    printf(_IMINI_TEST_E_RESET "\n\n"); \
    if (setjmp(_imini_test__jmpbuf_to_fail)) { /* Error handler */ \
      printf("\n" _IMINI_TEST_E_RED "Test failed!" _IMINI_TEST_E_RESET "\n"); \
      return; \
    } else {

/// End of test case. Prints "All test passed" message if
/// everything is okay.
#define imini_test_case_end \
    } printf("\n" _IMINI_TEST_E_GREEN "All tests passed" _IMINI_TEST_E_RESET "\n"); }

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

// Different status messages printed by tester.
// Must be the same length to print properly.
// (you can pad them with spaces to achive that)
#define _IMINI_TEST_PROGRESS_MSG _IMINI_TEST_E_GRAY  "...." _IMINI_TEST_E_RESET
#define _IMINI_TEST_FAIL_MSG     _IMINI_TEST_E_RED   "FAIL" _IMINI_TEST_E_RESET
#define _IMINI_TEST_OK_MSG       _IMINI_TEST_E_GREEN "PASS" _IMINI_TEST_E_RESET

/// \internal
/// Check what condition is true, if not print something with printer.
/// \param condition - Boolean condition to check
/// \param fmt       - Label for this check, with printf syntax
/// \param printer   - Block of code to be called when condition fails,
///                    like `{ printf("We are doomed!\n"); }`
/// \param VA_ARGS   - Arguments, required by formatted label
#define _imini_test_assert_base(condition, fmt, printer, ...) { \
    printf(" %s " fmt, _IMINI_TEST_PROGRESS_MSG __VA_OPT__(,) __VA_ARGS__); \
    if ((condition)) { \
      printf("\r %s\n", _IMINI_TEST_OK_MSG); \
    } else { \
      printf("\r %s\n", _IMINI_TEST_FAIL_MSG); \
      printf("\n"); \
      printer; \
      printf("\n"); /* Some space after end of test section */ \
      longjmp(_imini_test__jmpbuf_to_fail, 0); \
    } \
  }

/// Assert what condition is `true`
/// \param condition - Condition to check
/// \param fmt - Label of this assert, like `"The value must be positive"`
/// \param VA_ARGS - Arguments for `fmt`
#define imini_test_assert(condition, fmt, ...) \
    _imini_test_assert_base(condition, fmt,\
      { printf("   " _IMINI_TEST_E_RED "%s" _IMINI_TEST_E_RESET " is not true\n", #condition); }, __VA_ARGS__)

/// \internal
/// Compare `a` and `b` using given comparator and fail this test if they are not equal.
/// \param condition - Comparator function macro with signature `bool COMPARE(a, b)`
/// \param a     - First value
/// \param b     - Second value
/// \param arg   - Extra argument to compartor macro
/// \param label - Label of the assert
/// \param VA_ARGS - Format 
#define _imini_test_assert_equal_base(condition, a, b, arg, fmt, ...) { \
    typeof(a) _imini_test__a = (a); \
    typeof(b) _imini_test__b = (b); \
    _imini_test_assert_base( \
      condition(_imini_test__a, _imini_test__b, arg), \
      fmt, \
      { \
        printf("   " _IMINI_TEST_E_RED "%s" _IMINI_TEST_E_RESET " (a) = ", #a); \
        _imini_test_print(_imini_test__a); \
        printf("\n   " _IMINI_TEST_E_RED "%s" _IMINI_TEST_E_RESET " (b) = ", #b); \
        _imini_test_print(_imini_test__b); \
        printf("\n"); \
      } \
      __VA_OPT__(,) __VA_ARGS__ \
    ) \
  }

/// \internal
/// Comparator used in \ref imini_test_assert_equal
#define _imini_test_equality_usual(a, b, _) (a == b)

/// Check equality of `a` and `b` and fail this test if they are not equal.
/// \param a       - First value
/// \param b       - Second value
/// \param fmt     - Label of the assert
/// \param VA_ARGS - Arguments to printf(fmt, ...) for printing label
#define imini_test_assert_equal(a, b, fmt, ...) \
  _imini_test_assert_equal_base(_imini_test_equality_usual, a, b, _, fmt, __VA_ARGS__)

/// \internal
/// Comparator used in \ref imini_test_assert_somewhat_equal
#define _imini_test_equality_eps(a, b, eps) (a < b * (1 + eps) && a > b * (1 - eps))

/// Check if floats `a` and `b` are equal with given relative `epsilon`
/// \param a       - First value
/// \param b       - Second value
/// \param eps     - Epsilon.
/// \param fmt     - Label of the assert
/// \param VA_ARGS - Arguments to printf(fmt, ...) for printing label
#define imini_test_assert_somewhat_equal(a, b, eps, fmt, ...) { \
    typeof(eps) _imini_test__eps = (eps); \
    _imini_test_assert_equal_base(_imini_test_equality_eps, a, b, _imini_test__eps, fmt, __VA_ARGS__);\
  }

#endif

#if !defined(ISTD_IMINI_TEST_IMPLEMENTED) && defined(ISTD_IMINI_TEST_IMPLEMENTATION)
#define ISTD_IMINI_TEST_IMPLEMENTED

/// Just implement one global here...
jmp_buf _imini_test__jmpbuf_to_fail;

#endif

