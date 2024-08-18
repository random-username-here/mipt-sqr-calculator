#include <math.h>
#include "sqe/solve.h"

const double EPS = 0.0000001;
static int iszero(double v) { return fabs(v) < EPS; }

/// Решаем уравнение вида ax^2 + bx + c
solve_result sqe_solve(double a, double b, double c) {
  solve_result res;
  res.a = a; res.b = b; res.c = c;
  if (!iszero(a)) { // ax^2 + bx + c
    res.D = b * b - 4 * a * c;
    if (iszero(res.D)) // D = 0
      res.type = RESULT_SQR_ONE, res.x1 = -b/(2*a);
    else if (res.D < 0) // D < 0
      res.type = RESULT_SQR_NONE;
    else { // D > 0
      double sD = sqrt(res.D);
      res.type = RESULT_SQR_TWO;
      res.x1 = (-b - sD) / (2*a);
      res.x2 = (-b + sD) / (2*a);
    }
  } else if (!iszero(b)) // bx + c = 0
    res.type = RESULT_LINEAR, res.x1 = - c / b;
  else if (!iszero(c)) // c = 0, при том что c != 0
    res.type = RESULT_CONST_NONE;
  else // 0 = 0
    res.type = RESULT_CONST_ANY;
  return res;
}
