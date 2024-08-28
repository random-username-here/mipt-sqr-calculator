/**
 * \file
 * \brief Function to hexdump memory ranges
 */

#ifndef ISTD_XXD
#define ISTD_XXD

/// \brief Hexdump memory from `begin` (included) to `end` (not included)
///
/// Title can be `NULL` if it is not needed
void istd_xxd(const void* begin, const void* end, const char* title);

#endif // ISTD_XXD
