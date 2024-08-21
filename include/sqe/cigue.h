#pragma once
#include "cigue/base.h"
#include "sqe/solve.h"

// Запускает UI на cigue.
void sqe_cigue_ui();

/// Рисует решение.
///
/// Запускать в `cigue_column`.
///
/// @param [in] gui - Интерефейс, в котором рисовать решение
/// @param [in] res - Решение, которое нужно рисовать. Можно получить используя [sqe_solve].
void sqe_cigue_paint_solution(cigue_state* gui, const solve_result *res);
