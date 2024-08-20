#include <math.h>
#include <assert.h>
#include "sqe/solve.h"

int iszero(double v);

void _solve_linear(solve_result* res, double b, double c);

void _solve_sqr(solve_result* res, double a, double b, double c);

////////////////////////////////////////////////////////////

const double EPS = 0.0000001;
int iszero(double v) { return fabs(v) < EPS; }

// bx + c = 0
void _solve_linear(solve_result* res, double b, double c) {

  assert(res);
  assert(isfinite(b));
  assert(isfinite(c));

  if (!iszero(b)) { // bx + c = 0
    res->type = RESULT_LINEAR;
    res->x1 = - c / b;
  } else if (!iszero(c)) // c = 0, при том что c != 0
    res->type = RESULT_CONST_NONE;
  else // 0 = 0
    res->type = RESULT_CONST_ANY;
}

// ax^2 + bx + c = 0, a != 0
void _solve_sqr(solve_result* res, double a, double b, double c) {

  assert(res);
  assert(isfinite(a));
  assert(isfinite(b));
  assert(isfinite(c));

  res->D = b * b - 4 * a * c;
  if (iszero(res->D)) // D = 0
    res->type = RESULT_SQR_ONE, res->x1 = -b/(2*a);
  else if (res->D < 0) // D < 0
    res->type = RESULT_SQR_NONE;
  else { // D > 0
    double sD = sqrt(res->D);
    res->type = RESULT_SQR_TWO;
    res->x1 = (-b - sD) / (2*a);
    res->x2 = (-b + sD) / (2*a);
  }
}

/// Решаем уравнение вида ax^2 + bx + c
solve_result sqe_solve(double a, double b, double c) {

  assert(isfinite(a));
  assert(isfinite(b));
  assert(isfinite(c));

  solve_result res = { .a = a, .b = b, .c = c };

  if (!iszero(a)) // ax^2 + bx + c
    _solve_sqr(&res, a, b, c); 
  else
    _solve_linear(&res, b, c);

  return res;
}
