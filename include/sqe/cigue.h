#pragma once
#include "cigue/base.h"
#include "sqe/solve.h"

#define ESC_GRAY "\x1b[90m"
#define ESC_RED "\x1b[91m"
#define ESC_RESET "\x1b[0m"
#define ESC_BOLD "\x1b[1m"

// Запускает UI на cigue.
void sqe_cigue_ui();

/// Рисует решение.
/// Запускать в `cigue_column`.
void sqe_cigue_paint_solution(cigue_state* gui, const solve_result *res);
