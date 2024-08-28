#include "istd/xxd.h"
#include <stddef.h>

int main () {

  const char s[] = "Some more text which is very long and can be xxd-ed, with \x1b \1 \2 \3 nonprintable chars!";

  istd_xxd(s, s + sizeof(s), "Cool hexdump");

  char long_s[1024];
  for (size_t i = 0; i < 1024; ++i)
    long_s[i] = (char) (i % 256);

  istd_xxd(long_s, long_s + sizeof(long_s), "Long string");

  return 0;
}
