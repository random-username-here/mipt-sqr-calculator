/// 
/// Argument parsing single-header library for C.
///
///

enum iarg_usage {
  IARG_FLAG,
  IARG_VALUE,
  IARG_BOTH
};

/// Specification for one argument
struct iarg_spec {
  /// Names of this argument. To specify multiple names
  /// write them separated by space.
  /// For example, `.arg_names = "-h --help /h"`.
  /// Parser will match every argument containing any
  /// of specified names until `=`.
  /// So `--help=foo` will match, but may emit error
  /// about wrong usage of that argument.
  const char* arg_names;


};
