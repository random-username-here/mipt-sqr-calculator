/**
 * \file
 * \brief Primitive single-header-file testing library.
 *
 * Implement this using `ISTD_MINI_TEST_IMPLEMENTATION` macro before include in one file.
 * 
 * Syntax for this is:
 * ```c
 * imini_test_case("Test of abs()") {
 *   imini_test_assert(something, "Something shall be true");
 *   imini_test_assert_equal(get_answer(), 42, "Answer must be an actual answer");
 * }
 * ```
 */

#ifndef ISTD_MINI_TEST
#define ISTD_MINI_TEST
 

#include "istd/macro.h"
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>

/// Printf(), which strips escape sequences when output
/// is redirected into file
void imini_test_printf_escaped(const char* fmt, ...);

// ASCII escape codes for use inside `imini-test.h`
#define _IMINI_TEST_E_GRAY   "\x1b[90m"
#define _IMINI_TEST_E_RED    "\x1b[91m"
#define _IMINI_TEST_E_GREEN  "\x1b[92m"
#define _IMINI_TEST_E_YELLOW "\x1b[93m"
#define _IMINI_TEST_E_BOLD   "\x1b[1m"
#define _IMINI_TEST_E_RESET  "\x1b[0m"

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
  imini_test_printf_escaped(_IMINI_TEST_E_GRAY "\n## " _IMINI_TEST_E_RESET \
         _IMINI_TEST_E_BOLD fmt "\n\n" _IMINI_TEST_E_RESET __VA_OPT__(,) __VA_ARGS__)

/// Generate code for beginning of test case
/// It will return something like this:
/// ```c
/// static void user_fn(); // predefenition
///
/// // Function which gets called at start
/// static void [[constructor]] test_fn() {
///   // ... print header
///   if (setjmp(buffer_to_jump_to_if_failed)) {
///     // ... print "we failed"
///   } else {
///     user_fn();
///     // ... print "tests passed"
///   }
/// }
///
/// static void user_fn() /* and here macro ends */
/// ```
#define _imini_test_case_code(name, test_fn, user_fn) \
  /* Predefine user function */ \
  static void user_fn (); \
  /* Function with test case handling */ \
  _imini_test_ctor static void test_fn () {  \
    \
    /* Store test name in temp. variable */ \
    const char* _imini_test__name = (name); \
    const size_t _imini_test__name_len = strlen(_imini_test__name); \
    \
    /* Print header */\
    imini_test_printf_escaped("\n" \
        _IMINI_TEST_E_GRAY _IMINI_TEST_HEADER_PREFIX _IMINI_TEST_E_RESET \
        _IMINI_TEST_E_BOLD "%s" _IMINI_TEST_E_RESET \
        _IMINI_TEST_E_GRAY _IMINI_TEST_HEADER_SUFFIX, _imini_test__name); \
    for (size_t i = sizeof(_IMINI_TEST_HEADER_PREFIX) \
                  + sizeof(_IMINI_TEST_HEADER_SUFFIX) \
                  + _imini_test__name_len; i < _IMINI_TEST_HEADER_WIDTH; ++i) \
      imini_test_printf_escaped("-"); \
    imini_test_printf_escaped(_IMINI_TEST_E_RESET "\n\n"); \
    \
    /* Setjmp if */ \
    if (setjmp(_imini_test__jmpbuf_to_fail)) { /* Error handler */ \
      imini_test_printf_escaped("\n" _IMINI_TEST_E_RED "Test failed!" _IMINI_TEST_E_RESET "\n"); \
      return; \
    } else { \
      user_fn();\
      imini_test_printf_escaped("\n" _IMINI_TEST_E_GREEN "All tests passed" _IMINI_TEST_E_RESET "\n");\
    }\
  }\
  \
  /* Function with user code */ \
  static void user_fn ()

/// This step is requred because otherwise __COUNTER__ would
/// expand twice.
#define _imini_test_case_code_from_number(name, counter) \
  _imini_test_case_code(\
    name,\
    imacro_concat(_imini_test_generated__, counter),\
    imacro_concat(_imini_test_generated_user__, counter)\
  )

/// Begin test case, printing nice header.
/// \param name - Name of the test case, a string.
#define imini_test_case(name) \
  _imini_test_case_code_from_number(name, __COUNTER__)

//------- Generic print function -------------------------//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal" // Somehow found in stringization of bool

// This is one big HACK, because C is not
// suited for this thing. I know that.
// TODO: figure out how to fit bool here

#define _imini_test_print_fmt(value) \
  _Generic((value), \
    char: "%c", \
    signed char: "%hhd", \
    unsigned char: "%hhu", \
    signed short: "%hd", \
    unsigned short: "%hu", \
    signed int: "%d", \
    unsigned int: "%u", \
    long int: "%ld", \
    unsigned long int: "%lu", \
    long long int: "%lld", \
    unsigned long long int: "%llu", \
    float: "%f", \
    double: "%f", \
    long double: "%Lf", \
    /*_Bool: "%s",*/\
    void *: "%p",\
    char *: "`%s`",\
    default: "<value of unknowntype>%c" /* HACK: this prints extra space, so arg to imini_test_printf_escaped will be used */ \
  )

#define _imini_test_print_value(value) \
  _Generic((value), /* HACK: gcc thinks _Bool case can be evaluated with float value */ \
    /*_Bool: (value ? "true" : "false"),*/\
    char: value, \
    signed char: value, \
    unsigned char: value, \
    signed short: value, \
    unsigned short: value, \
    signed int: value, \
    unsigned int: value, \
    long int: value, \
    unsigned long int: value, \
    long long int: value, \
    unsigned long long int: value, \
    float: value, \
    double: value, \
    long double: value, \
    void *: value,\
    char *: value,\
    default: ' '\
  )

/// \internal
/// Write given value to stdout.
/// \param value - Value to print. Can be anything.
#define _imini_test_print(value) \
  imini_test_printf_escaped(_imini_test_print_fmt(value), _imini_test_print_value(value))

#pragma GCC diagnostic pop

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
/// \param fmt       - Label for this check, with imini_test_printf_escaped syntax
/// \param printer   - Block of code to be called when condition fails,
///                    like `{ imini_test_printf_escaped("We are doomed!\n"); }`
/// \param VA_ARGS   - Arguments, required by formatted label
#define _imini_test_assert_base(condition, fmt, printer, ...) { \
    imini_test_printf_escaped(" " _IMINI_TEST_PROGRESS_MSG " " fmt __VA_OPT__(,) __VA_ARGS__); \
    if ((condition)) { \
      imini_test_printf_escaped("\r " _IMINI_TEST_OK_MSG "\n"); \
    } else { \
      imini_test_printf_escaped("\r " _IMINI_TEST_FAIL_MSG "\n"); \
      imini_test_printf_escaped("\n"); \
      printer; \
      imini_test_printf_escaped("\n"); /* Some space after end of test section */ \
      longjmp(_imini_test__jmpbuf_to_fail, 0); \
    } \
  }

/// \brief Assert what condition is `true`
/// \param condition - Condition to check
/// \param fmt - Label of this assert, like `"The value must be positive"`
/// \param VA_ARGS - Arguments for `fmt`
#define imini_test_assert(condition, fmt, ...) \
    _imini_test_assert_base(condition, fmt,\
      { imini_test_printf_escaped("   " _IMINI_TEST_E_RED "%s" _IMINI_TEST_E_RESET " is not true\n", #condition); }, __VA_ARGS__)

/// \internal
/// Compare `a` and `b` using given comparator and fail this test if comparator
/// returned false.
/// \param condition - Comparator function macro with signature `bool COMPARE(a, b)`
/// \param a     - First value
/// \param b     - Second value
/// \param arg   - Extra argument to compartor macro
/// \param note  - Note to print above values of `a` and `b`, like `"Those values are not equal"`
/// \param label - Label of the assert
/// \param VA_ARGS - Format 
#define _imini_test_comparator_base(condition, a, b, arg, note, fmt, ...) { \
    typeof(a) _imini_test__a = (a); \
    typeof(b) _imini_test__b = (b); \
    _imini_test_assert_base( \
      condition(_imini_test__a, _imini_test__b, arg), \
      fmt, \
      { \
        imini_test_printf_escaped("   %s:\n", (note)); \
        imini_test_printf_escaped("   " _IMINI_TEST_E_RED "%s" _IMINI_TEST_E_RESET " (a) = " _IMINI_TEST_E_YELLOW, #a); \
        _imini_test_print(_imini_test__a); \
        imini_test_printf_escaped("\n   " _IMINI_TEST_E_RED "%s" _IMINI_TEST_E_RESET " (b) = " _IMINI_TEST_E_YELLOW, #b); \
        _imini_test_print(_imini_test__b); \
        imini_test_printf_escaped(_IMINI_TEST_E_RESET "\n"); \
      } \
      __VA_OPT__(,) __VA_ARGS__ \
    ) \
  }

/// \internal
/// Comparator used to check equality/inequalty with given operator
#define _imini_test_operator(a, b, op) (a op b)

/// \brief Check equality of `a` and `b` and fail this test if they are not equal.
/// \param a       - First value
/// \param b       - Second value
/// \param fmt     - Label of the assert
/// \param VA_ARGS - Arguments to imini_test_printf_escaped(fmt, ...) for printing label
#define imini_test_assert_equal(a, b, fmt, ...) \
  _imini_test_comparator_base( \
      _imini_test_operator, a, b, ==, \
      "Those values should've been equal", \
      fmt, __VA_ARGS__ \
  )

/// \brief Check if `a < b`.
#define imini_test_assert_lt(a, b, fmt, ...) \
  _imini_test_comparator_base( \
      _imini_test_operator, a, b, <, \
      "A should've been less than B", \
      fmt, __VA_ARGS__ \
  )

/// \brief Check if `a > b`.
#define imini_test_assert_gt(a, b, fmt, ...) \
  _imini_test_comparator_base( \
      _imini_test_operator, a, b, >, \
      "A should've been greater than B",\
      fmt, __VA_ARGS__ \
  )

/// \brief Check if `a >= b`.
#define imini_test_assert_ge(a, b, fmt, ...) \
  _imini_test_comparator_base( \
      _imini_test_operator, a, b, >=,\
      "A should've been >= B", \
      fmt, __VA_ARGS__\
  )

/// \brief Check if `a <= b`.
#define imini_test_assert_le(a, b, fmt, ...) \
  _imini_test_comparator_base(\
      _imini_test_operator, a, b, <=, \
      "A should've been <= B",\
      fmt, __VA_ARGS__\
  )

/// \brief Check if `a != b`.
/// \todo Maybe use other printer?
#define imini_test_assert_ne(a, b, fmt, ...) \
  _imini_test_comparator_base(\
      _imini_test_operator, a, b, !=,\
      "Those values should've been diffrent, but they are not",\
      fmt, __VA_ARGS__\
  )

#define _imini_generic_abs(X)\
  (_Generic((X), \
      long double: fabsl, \
          default: fabs,  \
            float: fabsf  \
  )(X))

/// \internal
/// Comparator used in \ref imini_test_assert_somewhat_equal
#define _imini_test_equality_eps(a, b, eps) \
  (_imini_generic_abs(a - b) < eps)

/// \brief Check if floats `a` and `b` are equal with given absolute `epsilon`
/// \param a       - First value
/// \param b       - Second value
/// \param eps     - Epsilon.
/// \param fmt     - Label of the assert
/// \param VA_ARGS - Arguments to imini_test_printf_escaped(fmt, ...) for printing label
#define imini_test_assert_somewhat_equal(a, b, eps, fmt, ...) { \
    typeof(eps) _imini_test__eps = (eps); \
    _imini_test_comparator_base(\
        _imini_test_equality_eps, a, b, _imini_test__eps,\
        "Those values should've been approximately equal",\
        fmt, __VA_ARGS__\
    );\
  }

#define _imini_test_equality_strcmp(a, b, _) (!strcmp(a, b))

/// \brief Checks if two given strings are equal.
#define imini_test_assert_str_equal(a, b, fmt, ...) { \
    _imini_test_comparator_base(\
        _imini_test_equality_strcmp, a, b, _,\
        "Those strings should've been equal",\
        fmt, __VA_ARGS__\
    );\
  }


#endif

#if !defined(ISTD_MINI_TEST_IMPLEMENTED) && defined(ISTD_MINI_TEST_IMPLEMENTATION)
#define ISTD_MINI_TEST_IMPLEMENTED

/// Just implement one global here...
jmp_buf _imini_test__jmpbuf_to_fail;

#endif

