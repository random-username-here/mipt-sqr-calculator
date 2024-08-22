#include "istd/mini-test.h"
#include "sqe/solve.h"
#include <assert.h>
#include <stdarg.h>

/// NOTE: calculate actually expected error
const double EXPECTED_EPS = 1e-5;

static void check_linear(double expected, solve_result res, const char* name) {
  assert(name);
  
  imini_test_assert_equal(
      res.type, RESULT_LINEAR,
      "%s: solution must have a proper type", name
  );
  imini_test_assert_somewhat_equal(
      res.x1, expected, EXPECTED_EPS,
      "%s: X must be as expected", name);
}

/// Check roots of square equation solution.
/// Double solutions are expected as variadic args.
static void check_sqr(int num_res, solve_result res, const char* name, ...) {
  assert(num_res >= 0);
  assert(num_res <= 2);
  assert(name);

  imini_test_assert_equal(
      res.type, RESULT_SQR_NONE + num_res,
      "%s: solution must have a proper type", name
  );

  va_list args;
  va_start(args, name);
  const double* solutions[2] = { &res.x1, &res.x2 };

  for (int i = 0; i < num_res; ++i)
    imini_test_assert_somewhat_equal(
        *solutions[i], va_arg(args, double), EXPECTED_EPS,
        "%s: X solution %d must be as expected", name, i+1);

  va_end(args);
}


imini_test_case("Square solver")
  solve_result res;
  
  //------------------------------------------------------//
  imini_test_header("Const cases (a = 0 and b = 0)");

  res = sqe_solve(0, 0, 0);
  imini_test_assert_equal(res.type, RESULT_CONST_ANY,
      "When x = anything, result type shall be correct");

  res = sqe_solve(0, 0, 1);
  imini_test_assert_equal(res.type, RESULT_CONST_NONE,
      "When x = nothing, result type shall be correct");

  //------------------------------------------------------//
  imini_test_header("Linear equation cases");

  check_linear( 0, sqe_solve(0, 1, 0), "x + 0 = 0");
  check_linear(-1, sqe_solve(0, 1, 1), "x + 1 = 0");
  check_linear( 1, sqe_solve(0, 1, -1), "x + 0 = 0");
  check_linear(-0.571287, sqe_solve(0, 7893.3, 4509.34), "7893.3 x + 4509.34 = 0");

  //------------------------------------------------------//
  imini_test_header("Square equations : cases when there are no solution");

  //check_sqr(0, sqe_solve(), "7893.3 x + 4509.34 = 0");

imini_test_case_end


