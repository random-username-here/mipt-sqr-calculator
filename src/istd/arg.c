#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "istd/arg.h"

/// Check if character is end of argument in `arg_names`.
static bool is_arg_end(char c) {
  return c == '\0' || c == ' ';
}

/// Check if argument from `arg_names` matches given `argv` value
/// \param spec - Argument as defined in `arg_names`.
/// \param arg  - Value from `argv`
static char* match(const char* spec, char* arg) {
  while (!is_arg_end(*spec) && *spec == *arg)
    ++spec, ++arg;
  if(is_arg_end(*spec) && (*arg == '=' || *arg == '\0'))
    return arg;
  else 
    return NULL;
}

const char* mode_to_str(iarg_type type) {
  switch (type) {
    case IARG_FLAG: return "flag (like --help)";
    case IARG_VALUE: return "value (like --output=foobar)";
    case IARG_BOTH: return "flag or value";
    default: assert(0);
  }
}

int iarg_parse(int argc, char** argv,
                iarg_spec* spec, size_t num_args,
                void* state) {

  assert(argc > 0);
  assert(argv);

  for (int i = 1; i < argc; ++i) {
    assert(argv[i]);

    bool matched = false;
    for (size_t arg = 0; !matched && arg < num_args; ++arg) {
      assert(spec[arg].callback);
      assert(spec[arg].arg_names);

      bool prev_space = true;

      for (const char* c = spec[arg].arg_names; *c != '\0'; ++c) {
        if (prev_space && *c != ' ') {
          // We are at beginning of a flag defenition
          char* value_begin = match(c, argv[i]);
          
          if (value_begin) {
            // It matched
            iarg_type type = *value_begin == '=' ? IARG_VALUE : IARG_FLAG;
            
            if (! (spec[arg].type & type)) {
              // But user used it wrong...
              *value_begin = '\0';
              fprintf(stderr, "Wrong usage of argument `%s`\n", value_begin);
              fprintf(stderr, "It must be used as %s, but you used it as %s\n",
                      mode_to_str(spec[arg].type), mode_to_str(type));
              return IARG_WRONG_ARG;
            }

            // Call the callback
            spec[arg].callback(state, spec[arg].user_param, *value_begin == '=' ? value_begin+1 : NULL);
            matched = true;
            break;
          }
        }
        prev_space = *c == ' ';
      }
    }

    if (!matched) {
      fprintf(stderr, "Unknown argument `%s`\n", argv[i]);
      return IARG_WRONG_ARG;
    }
  }

  return 0;
}
