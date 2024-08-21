//!
//! Решатель квадратных уравнений
//!

#pragma once

/// Тип решения
enum solve_result_type { // TODO: naming, solve and result in the same name feels tautological
  /// Решением является любое число
  RESULT_CONST_ANY,
  /// Ни одного решения нет, т.к. *бестолковый* пользователь ввёл
  /// что-то вроде 1 = 2.
  RESULT_CONST_NONE,
  /// Линейное уравнение
  RESULT_LINEAR, // TODO: can you think of a way to also solve linear equations separately
  /// Уравнение квадратное, с одним решением
  RESULT_SQR_NONE,
  /// Квадратное с одним, $D = 0$
  RESULT_SQR_ONE,
  /// Нормальное квадратное с двумя решением.
  RESULT_SQR_TWO
};


// TODO: theoretically possible:
// TODO: enum E0 { E0_A, E0_B, E0_C, A_SIZE }, enum E1 { E1_A = A_SIZE, ... };

struct solve_result { // TODO: typedef struct { ... } name;
  enum solve_result_type type;
  double a, b, c;
  double x1, x2;
  double D;
};

typedef struct solve_result solve_result;

/// Решаем уравнение вида ax^2 + bx + c
///
/// @param [in] a - Коэффициент a
/// @param [in] b - Коэффициент b
/// @param [in] c - Коэффициент c
/// @returns        Решение уравнения
solve_result sqe_solve(double a, double b, double c); // TODO: be more explicit with the name, don't try hard to save on typing
