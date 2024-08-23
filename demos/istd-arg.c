#include <stdio.h>
#include "istd/arg.h"

#define BLUE "\x1b[94m"
#define GRAY "\x1b[90m"
#define RESET "\x1b[0m"

void handle_arg(void* state, void* user, const char* value) {
  printf(BLUE "%6s" GRAY " = " RESET "%s\n", (char*) user, value);
}

iarg_spec arg_spec[] = {
  { .arg_names="/i -i --input", .type=IARG_VALUE, .callback = handle_arg, .user_param="input" },
  { .arg_names="/h -h --help", .type=IARG_FLAG, .callback = handle_arg, .user_param="help" },
  { .arg_names="--weird-flag", .type=IARG_BOTH, .callback = handle_arg, .user_param="flag" },
};

int main (int argc, char** argv) {
  int err = iarg_parse(argc, argv, arg_spec, sizeof(arg_spec) / sizeof(iarg_spec), NULL);
  if (!err) {
    printf("All arguments read succesfully\n");
  } else {
    printf("Error while reading args\n");
  }
}
