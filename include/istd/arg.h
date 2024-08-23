/**
 * \file
 * \brief A simple command line argument parser
 */

#ifndef ISTD_ARG
#define ISTD_ARG
#include <stddef.h>

/// Type of the argument,
/// used when defining list of args
///
/// You may OR them to combine.
typedef enum {
  
  /// This argument must be a flag (like `--help`)
  IARG_FLAG = 1,

  /// This argument must be a value (like `--output=foo.bar`) 
  IARG_VALUE = 2,

  /// This argument can be both
  IARG_BOTH = 3,

} iarg_type;

/// Description of one argument
typedef struct {

  /// Names of this argument, as space separated string.
  const char* arg_names;
  
  /// Type of the argument
  iarg_type type;

  /// Param to pass to the function
  void* user_param;

  /// Callback
  /// Gets `NULL` as value when this argument is a flag.
  void (*callback)(void* state, void* user_param, const char* value);

} iarg_spec;

/// Value returned by `iarg_parse` after it finds some error and prints message
/// aboutit.
#define IARG_WRONG_ARG 1

/// \brief Parse command line arguments
/// \param argc     Number of arguments, as passed to `main()`
/// \param argv     Values of arguments, again, as passed to `main`
/// \param spec     List of argument descriptions
/// \param num_args Number of arguments descrbed in `spec`
/// \param state    State to pass to callbacks
int iarg_parse(int argc, char** argv,
               iarg_spec* spec, size_t num_args,
               void* state);

#endif
