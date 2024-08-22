#include <execinfo.h> // glibc-s headers, not portable
#include <stdio.h>
#include "istd/except.h"

#define TRACE_LEN 30

struct my_exception {
  const char* name;
  void* backtrace[TRACE_LEN];
  int num_symbols;
};

static void print_exception(struct my_exception* ex) {
  fprintf(stderr, "\n  \x1b[91m!!! %s !!!\x1b[0m\n", ex->name);
  char** strings = backtrace_symbols(ex->backtrace, ex->num_symbols);
  char* c = "NOT SCANNED";
  if (strings) {
    for (int i = 0; i < ex->num_symbols; ++i) {
      char* file = c, *func = c, *offset = c;
      sscanf(strings[i], "%m[^(]%m[^+]+%m[^)]", &file, &func, &offset);
      fprintf(stderr, "\x1b[90m  in \x1b[92m%s\x1b[90m at \x1b[94m%s + %s\x1b[0m\n", file, func+1, offset);
      free(file); free(func); free(offset);
    }
    free(strings);
  } else {
    fprintf(stderr, "<unable to get backtrace>");
  }
}

static void free_exception(struct my_exception* ex) {
  free(ex);
}

static void throw_ex(const char* name) {
  struct my_exception* ex = 
    (struct my_exception*) malloc(sizeof(struct my_exception));
  ex->name = name;
  ex->num_symbols = backtrace(ex->backtrace, TRACE_LEN);
  iexcept_throw(
      ex,
      (iexcept_cleaner) &free_exception,
      (iexcept_printer) &print_exception
  );
}

static void my_fn () {
  throw_ex("Scary exception");
}

int main () {
  itry {
    my_fn();
  } icatch(ex) {
    throw_ex("Exception too scary to handle");
  }  
}
