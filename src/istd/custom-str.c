#include <assert.h>
#include <stdlib.h>
#include "istd/custom-str.h"

char* i_strchr(char* str, char ch) {
  
  assert(str);

  for (; *str != '\0'; ++str)
    if (*str == ch)
      return str;
  return NULL;
}

size_t i_strlen(const char * str) {

  assert(str);

  size_t len = 0;
  while(*(str++)) ++len;
  return len;
}

#define SIZE_T_MAX ((size_t) -1)

void i_strncpy(const char *restrict src, char *restrict dest, size_t n) {

  assert(src);
  assert(dest);

  size_t i = 0;
  for (; src[i] != '\0' && i < n-1; ++i)
    dest[i] = src[i];

  dest[i] = '\0';
}

void i_strcpy(const char *restrict src, char *restrict dest) {
  i_strncpy(src, dest, SIZE_T_MAX);
}

void i_strncat(char *restrict target, const char* restrict suffix, size_t n) {

  assert(target);
  assert(suffix);

  size_t len = i_strlen(target);

  target += len;
  n -= len;

  i_strncpy(suffix, target, n);
}

void i_strcat(char *restrict target, const char* restrict suffix) {
  i_strncat(target, suffix, SIZE_T_MAX);
}

void i_fputs(FILE* f, const char* str) {
  while (*str)
    fputc(*(str++), f);
}

#define NOT_END 'a'

static int is_end(int c) {
  return c == EOF || c == '\n';
}

void i_fgets(FILE* f, char* out, size_t size_left) {

  assert(out);
  assert(size_left);

  --size_left; // Reserve space for \0

  int last = NOT_END;
  while (size_left && !is_end(last)) {
    *(out++) = (char) (last = fgetc(f));
    --size_left;
  }

  if (last == EOF) // Overwrite EOF, we don't want it
    *(out-1) = '\0';
  else // We got a normal ending
    *out = '\0';
}

void i_fgetsm(FILE* f, char** out, size_t avail_size) {
  
  assert(f);
  assert(out);

  if (*out == NULL) 
    avail_size = 0;

  size_t used = 0;
  int last = NOT_END;

  while (!is_end(last)) {
    if (used+2 > avail_size) { // Need space for \0!
      avail_size = avail_size * 3 / 2 + 2;
      *out = realloc(*out, avail_size);
    }
    (*out)[used++] = (char) (last = fgetc(f));
  }

  if (last == EOF)
    (*out)[used-1] = '\0';
  else
    (*out)[used] = '\0';
}

char* i_strdup(const char* original) {
  char* s = calloc(i_strlen(original)+1, 1);
  i_strcpy(original, s);
  return s;
}


