/**
 * \file
 * \brief Common macros used by other istd libs.
 */

#ifndef ISTD_IMACRO
#define ISTD_IMACRO

/// \private
#define _imacro_internal_concat2(a, b) a##b

/// Concat two names
/// \param a - First name
/// \param b - Second name
/// \returns - First name appended to second
///            name, without any separators
/// \example `imacro_concat(foo, bar) => foobar`
#define imacro_concat(a, b) _imacro_internal_concat2(a, b)

#endif
