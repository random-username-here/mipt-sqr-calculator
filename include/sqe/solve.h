//!
//! Решатель квадратных уравнений
//!

#pragma once

enum solve_result_type {
  RESULT_CONST_ANY,
  RESULT_CONST_NONE,
  RESULT_LINEAR,
  RESULT_SQR_NONE,
  RESULT_SQR_ONE,
  RESULT_SQR_TWO
};

struct solve_result {
  enum solve_result_type type;
  double a, b, c;
  double x1, x2;
  double D;
};

typedef struct solve_result solve_result;

/// Решаем уравнение вида ax^2 + bx + c
solve_result sqe_solve(double a, double b, double c);
