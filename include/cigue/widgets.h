//!
//! Основные виджеты cigue.
//! Математические формулы отдельно в math-widgets.h
//!

#pragma once
#include "cigue/base.h"
#include <stdio.h>

/// Подпись с заданным текстом.
/// Пользователь должен гарантировать, что
/// строка никуда не исчезнет до рендера.
///
/// Лучше не использовать, если нет чёткого понимания
/// зачем это.
void cigue_external_label(cigue_state* s, const char* text);

/// Просто подпись. Как текст можно использовать
/// любые строки, в том числе и временные.
void cigue_label(cigue_state* s, const char* text);

/// Подпись с содержимым, создаваевым printf-подобной функцией.
/// TODO
// void cigue_labelf(cigue_state* s, const char* fmt, ...);

/// То же самое, что `cigue_labelf`, только принимает `va_list`.
/// TODO
// void cigue_labelvf(cigue_state* s, const char* fmt, va_list args);

/// Начинаем колонку, заканчиваемую `cigue_end()`.
void cigue_begin_column(cigue_state* s, int spacing);
