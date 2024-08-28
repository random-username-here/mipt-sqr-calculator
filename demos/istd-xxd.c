#include "istd/xxd.h"

int main () {

  const char s[] = "Some more text which is very long and can be xxd-ed, with \x1b \1 \2 \3 nonprintable chars!";

  istd_xxd(s, s + sizeof(s));

  return 0;
}
