#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include "istd/xxd.h"

#define HEXDUMP_WIDTH 16
#define PRINT_LEN 512
#define GRAY "\x1b[90m"
#define GREEN "\x1b[92m"
#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"

void istd_xxd(const void *ptr, const void *end) {

  assert(end > ptr);

  uintptr_t begin = (uintptr_t) ptr;
  size_t len = (size_t) ((char*) end - (char*) ptr);

  size_t real_len = len;
  if (len > PRINT_LEN) len = PRINT_LEN;

  // Hexdump it out
  for (uintptr_t base = begin / HEXDUMP_WIDTH * HEXDUMP_WIDTH;
        base < begin + len;
        base += HEXDUMP_WIDTH) {  
    printf(GRAY "    %18p | " RESET, (void*) base);
  
    for (size_t col = 0; col < HEXDUMP_WIDTH; ++col) {
      if (base + col < begin || base + col >= begin+len) printf("   ");
      else printf("%02x ", (unsigned) *((unsigned char*) (base + col)));
    }

    printf(GRAY " | " RESET);
    
    for (size_t col = 0; col < HEXDUMP_WIDTH; ++col) {
      if (base + col < begin || base + col >= begin+len) printf(" ");
      else {
        char c = *((char*) (base + col));
        if (isprint(c)) printf(GREEN "%c" RESET, c);
        else printf(GRAY "." RESET);
      }
    }

    printf("\n");
  }
  if (real_len > len) {
    printf(GRAY "    ... and %zu bytes more ...\n" RESET, real_len - len);
  }


}
