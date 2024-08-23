#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "istd/memory.h"

#define HEXDUMP_WIDTH 16
#define PRINT_LEN 512
#define GRAY "\x1b[90m"
#define GREEN "\x1b[92m"
#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"

static void printer(void* ptr, size_t len, int used) {

  uintptr_t begin = (uintptr_t) ptr;

  printf(BOLD "\n - Segment [%p; %p) - %s\n\n" RESET, (void*) begin, (void*) (begin+len), (used ? "USED" : "FREE"));

  size_t real_len = len;
  if (len > PRINT_LEN) len = PRINT_LEN;

  // Hexdump it out
  for (uintptr_t base = begin / HEXDUMP_WIDTH * HEXDUMP_WIDTH; base < begin+len; base += HEXDUMP_WIDTH) {  
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

/// Fill given buffer with looped string,
/// 
/// ```c
/// char buf[21] = {};
/// fillstr(buf, 20, "str");
/// printf("%s", buf); // -> `strstrstr...`
/// ```
static void fillstr(char* buffer, size_t size, const char* str) {
  size_t len = strlen(str);
  for (size_t i = 0; i < size; ++i) {
    buffer[i] = str[i % len];
  }
}

int main () {
  im_get_info(&printer);
  
  printf("\n\nAllocating memory\n\n");
  char* buf1 = (char*) im_alloc(128);
  strcpy(buf1, "Hello world!");

  char* buf2 = (char*) im_alloc(1);
  *buf2 = '@';

  char* buf3 = (char*) im_alloc(128);
  for (size_t i = 0; i < 128; ++i)
    buf3[i] = 'A';

  char* buf5 = (char*) im_alloc(8192);
  fillstr(buf5, 8191, "buffer ");

  im_get_info(&printer);

  printf("\n\nFreed memory\n\n");
  
  im_free(buf1);
  im_free(buf3);
  im_get_info(&printer);

  printf("\n\nAlloccing again\n\n");

  buf1 = im_alloc(8);
  strcpy(buf1, "New one");

  im_get_info(&printer);

  //im_free(mem);
}
