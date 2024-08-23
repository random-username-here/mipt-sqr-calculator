#include "istd/mini-test.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#define STDOUT_FD 1
#define ESC_BEGIN '\x1b'

void imini_test_printf_escaped(const char* fmt, ...) {

  va_list args;
  va_start(args, fmt);

  if (isatty(STDOUT_FD))
    vprintf(fmt, args);
  else {
    size_t len = strlen(fmt), modified_len = 0;
    char *modified = (char*) calloc(1, len+1);
    
    bool escaped = false;
    for (size_t i = 0; i < len; ++i) {
      if (escaped && isalpha(fmt[i]))
        escaped = false;
      else if (!escaped && fmt[i] == ESC_BEGIN)
        escaped = true;
      else if (!escaped && fmt[i] != '\r')
        modified[modified_len++] = fmt[i];
    }
    modified[modified_len] = '\0';

    vprintf(modified, args);

    free(modified);
  }
}
