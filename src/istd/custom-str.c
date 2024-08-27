#include <assert.h>
#include <stdlib.h>
#include <string.h>
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

// ---- strstr -------------------------------------------//

char* i_strstr_dumb(char* haystack, const char* needle) {

  assert(haystack);
  assert(needle);

  size_t len = i_strlen(haystack),
         nlen = i_strlen(needle);

  if (nlen > len)
    return NULL;

  for (size_t i = 0; i <= len-nlen; ++i)
    if (!strncmp(haystack+i, needle, nlen))
      return haystack+i;

  return NULL;
}

typedef size_t hash_t;
const hash_t hash_exp = 257;
const hash_t hash_mod = (hash_t) (1e9+9);

static hash_t mul_hash(hash_t a, hash_t b) {
  long long unsigned rs = a % hash_mod;
  rs *= (b % hash_mod);
  rs %= hash_mod;
  return rs;
}

static hash_t add_hash(hash_t a, hash_t b) {
  return (a + b) % hash_mod;
}

static hash_t sub_hash(hash_t a, hash_t b) {
  return add_hash(a, hash_mod - b);
}

char* i_strstr_rabin_karp(char* haystack, const char* needle) {

  assert(haystack);
  assert(needle);

  size_t needle_len = i_strlen(needle),
         haystack_len = i_strlen(haystack);

  if (needle_len > haystack_len)
    return NULL;

  hash_t needle_hash = 0, right_factor = 1, left_factor = 1;
  hash_t left_hash = 0, right_hash = 0;

  for (size_t i = 0; i < needle_len; ++i) {
    needle_hash = add_hash(needle_hash, mul_hash((unsigned char) needle[i], right_factor));
    right_hash = add_hash(right_hash, mul_hash((unsigned char) haystack[i], right_factor));
    right_factor = mul_hash(right_factor, hash_exp);
  }

  // After all this:
  //  - needle_hash = hash(needle[0..needle_len])
  //  - left_hash = hash(haystack[0..0]) = 0
  //  - right_hash = hash(haystack[0..needle_len])
  // To advanc
  
  for (size_t i = 0; i <= haystack_len - needle_len; ++i) {

    // Check current substring
    if (sub_hash(right_hash, left_hash) == mul_hash(left_factor, needle_hash)) {
      if (!strncmp(haystack + i, needle, needle_len)) // There may be a collision 
        return haystack + i;
    }

    // Advance hashes
    right_hash = add_hash(right_hash, mul_hash((unsigned char) haystack[i+needle_len], right_factor));
    right_factor = mul_hash(right_factor, hash_exp);

    left_hash = add_hash(left_hash, mul_hash((unsigned char) haystack[i], left_factor));
    left_factor = mul_hash(left_factor, hash_exp);
  }

  return NULL;
}

char* i_strstr_boyer_moore_horspool(char * haystack, const char* needle) {

  const size_t OVERFLOWN = (size_t) -1;

  size_t needle_len = (size_t) i_strlen(needle),
      haystack_len = (size_t) i_strlen(haystack);

  size_t last_location[256] = { 0 };

  for (size_t i = 0; i < needle_len-1; ++i)
    last_location[(unsigned char) needle[i]] = i+1;

  for (size_t i = needle_len-1; i < haystack_len;) {

    //                 v i
    //   abeccacbadbabbad
    //             abbqa
    //                ^ mismatch_pos

    size_t needle_start = i - (needle_len-1);

    size_t mismatch_pos = needle_len-1;
    while (mismatch_pos != OVERFLOWN && // We compared all the needle, no diffrence found
           needle[mismatch_pos] == haystack[needle_start + mismatch_pos])
      --mismatch_pos;

    if (mismatch_pos == OVERFLOWN) // No mismatch found
      return haystack + needle_start;

    // else we have mismatched symbol at needle[mismatch_pos]
    i += needle_len - last_location[(unsigned char) haystack[i]];

  }

  return NULL;
}
