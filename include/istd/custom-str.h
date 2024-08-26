/**
 * \file
 * \brief Custom version of `string.h` functions
 */

#ifndef ISTD_CUSTOM_STR
#define ISTD_CUSTOM_STR

#include <stddef.h>
#include <stdio.h>

/// \brief Find first occurence of given character in the string
/// If nothing was found, returns `NULL`
char* i_strchr(char* str, char ch);

/// \brief Find length of given string
size_t i_strlen(const char * str);

/// \brief Copy string from `src` into `dest`,
///        truncating it to fit into `n` bytes if needed.
///
/// \note `n` includes space for zero byte
void i_strncpy(const char *restrict src, char *restrict dest, size_t n);

/// \brief Copy string from `src` into `dest`
/// \warn You must be sure what it will fit in `dest`!
void i_strcpy(const char *restrict src, char *restrict dest);

/// \brief Same as `i_strcat`, but target is limited to `n` bytes.
void i_strncat(char *restrict target, const char* restrict suffix, size_t n);

/// \brief Add suffix to given string
/// \warn You must be sure what it will fit.
void i_strcat(char *restrict target, const char* restrict suffix);

/// \brief Write given string to stdout
void i_fputs(FILE* f, const char* str);

/// \brief Read string from `f` until `\n` or `EOF` is found
///        into given buffer.
///
/// This will read up to `size-1` characters into buffer.
void i_fgets(FILE* f, char* out, size_t size); 

/// \brief Read string from stdin. If it does not fit into
///        buffer, buffer will be `realloc`-ed
void i_fgetsm(FILE* f, char** out, size_t original_size);

/// \brief Clone given string
char* i_strdup(const char* original);

#endif
