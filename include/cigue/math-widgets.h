//!
//! Виджеты для математических формул.
//! sup/sub тут нет, используйте символы вроде `x²`.
//! 

#pragma once
#include "cigue/base.h"

/// Дробь. Первый виджет числитель, второй знаменатель.
void cigue_math_begin_frac(cigue_state* s);

/// Квадратный корень.
void cigue_math_begin_sqrt(cigue_state* s);
