#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cigue/math-widgets.h"
#include "cigue/tty.h"
#include "cigue/base.h"
#include "cigue/widgets.h"
#include "sqe/cigue.h"

static double to_double(const char* buf) {
  if (*buf == 0) // ничего не введено
    return NAN;
  else {
    char* end;
    double res = strtod(buf, &end); 
    if (*end || isnan(res) || res == HUGE_VAL)
      return NAN;
    return res;
  }}

const char K_BACKSPACE[] = "\x7f";
const char K_LEFT[] = "\x1b[D";
const char K_RIGHT[] = "\x1b[C";

static void logo(cigue_state* gui) {
  cigue_begin_column(gui, 0);
    cigue_label(gui, "┌─┐┌─┐ ┬─┐  ┌─┐┌─┐ ┬ ┬┌─┐┌┬┐┬┌─┐┌┐┌┌─┐");
    cigue_label(gui, "└─┐│─┼┐├┬┘  ├┤ │─┼┐│ │├─┤ │ ││ ││││└─┐");
    cigue_label(gui, "└─┘└─┘└┴└─  └─┘└─┘└└─┘┴ ┴ ┴ ┴└─┘┘└┘└─┘");
    cigue_label(gui, "");
    cigue_label(gui, "\x1b[90m  [q]\x1b[0m Чтобы завершить программу");
    cigue_label(gui, "\x1b[90m  [<-], [->]\x1b[0m Чтобы двигаться между полями ввода");
  cigue_end(gui);
}

static double text_input(cigue_state* gui, char* buf, size_t maxlen,
                         bool focused, char* keypress) {

  // Обрабатываем ввод текста
  if (focused) {
    size_t len = strlen(buf);
    if (len > 0 && !strcmp(keypress, K_BACKSPACE))
      buf[len-1] = 0; // TODO: стираем юникод.
    if (strlen(keypress) == 1 && len < maxlen && isprint(*keypress)) {
      buf[len] = *keypress;
      buf[len+1] = 0;
    }
  }

  double dbl = to_double(buf);

  // Риусуем UI
  cigue_begin_border(gui);
    cigue_labelf(gui, "%s%20s%s %s\x1b[0m", (isnan(dbl) ? "\x1b[91m" : ""),
        buf, (focused ? "█" : " "), (isnan(dbl) ? "x" : " "));
  cigue_end(gui);

  return dbl;
}

const double EPS = 0.0000001;
static int iszero(double v) { return fabs(v) < EPS; }

static void solve_const(cigue_state* gui, double c) {
  // c = 0
  if (iszero(c)) {
    cigue_label(gui, "0 = 0");
    cigue_label(gui, "⇒ x любое");
  } else {
    cigue_labelf(gui, "%lf = 0", c);
    cigue_label(gui, "такое невозможно, так что x = ∅");
  }
}

static void solve_linear(cigue_state* gui, double b, double c) {
  // bx + c = 0
  double res = - c / b;
  cigue_begin_row(gui, 0);
    cigue_label(gui, "x = - ");
    cigue_math_begin_frac(gui);
      cigue_label(gui, "c");
      cigue_label(gui, "b");
    cigue_end(gui);
    cigue_label(gui, " = - ");
    cigue_math_begin_frac(gui);
      cigue_labelf(gui, "%lf", c);
      cigue_labelf(gui, "%lf", b);
    cigue_end(gui);
    cigue_labelf(gui, " = %lf", res);
  cigue_end(gui);
}

static void solve_sqr(cigue_state* gui, double a, double b, double c) {
  double D = b * b - 4 * a * c;

  // D = ...
  cigue_begin_row(gui, 0);
    cigue_label(gui, "D");
    cigue_begin_column(gui, 0);
      cigue_label(gui, " = b² - 4ac");
      cigue_labelf(gui, " = (%lf)² - 4 * %lf * %lf", b, a, c);
      cigue_labelf(gui, " = %lf", D);
    cigue_end(gui);
  cigue_end(gui);

  if (iszero(D)) { // D = 0
    cigue_label(gui, "D = 0 ⇒ решение одно");

    cigue_begin_row(gui, 0);
      cigue_label(gui, "x = ");
      cigue_math_begin_frac(gui);
        cigue_label(gui, "-b");
        cigue_label(gui, "2a");
      cigue_end(gui);
      cigue_label(gui, " = ");
      cigue_math_begin_frac(gui);
        cigue_labelf(gui, "%lf", -b);
        cigue_labelf(gui, "2 * %lf", a);
      cigue_end(gui);
      cigue_labelf(gui, " = \x1b[1m%lf\x1b[0m", -b / 2 / a);
    cigue_end(gui);

  } else if (D > 0) {
    cigue_label(gui, "D > 0 ⇒ решения два");

    double x1 = (-b - sqrt(D)) / (2 * a);
    double x2 = (-b + sqrt(D)) / (2 * a);

    // Гигантское вычисление x.
    // Нужно переделать синтаксис cigue.
    cigue_begin_row(gui, 0);
      cigue_label(gui, "x");
      cigue_begin_column(gui, 0);
        // Первая строка - формула
        cigue_begin_row(gui, 0);
          cigue_label(gui, " = ");
          cigue_math_begin_frac(gui);
            cigue_begin_row(gui, 0);
              cigue_label(gui, "-b ± ");
              cigue_math_begin_sqrt(gui);
                cigue_label(gui, "D");
              cigue_end(gui);
            cigue_end(gui);
            cigue_label(gui, "2a");
          cigue_end(gui);
        cigue_end(gui);
        // Вторая строка - формула с значениями
        cigue_begin_row(gui, 0);
          cigue_label(gui, " = ");
          cigue_math_begin_frac(gui);
            cigue_begin_row(gui, 0);
              cigue_labelf(gui, "%lf ± ", -b);
              cigue_math_begin_sqrt(gui);
                cigue_labelf(gui, "%lf", D);
              cigue_end(gui);
            cigue_end(gui);
            cigue_labelf(gui, "2 * %lf", a);
          cigue_end(gui);
        cigue_end(gui);
        // Пропустим строку, т.к. выглядит слишком склеенно.
        cigue_label(gui, "");
        // Третья строка - ответы.
        cigue_labelf(gui, " = \x1b[1m{%lf; %lf}\x1b[0m", x1, x2);
      cigue_end(gui);
    cigue_end(gui);

  } else if (D < 0) {
    cigue_label(gui, "D < 0 ⇒ решений нет");
    cigue_label(gui, "x = ∅");
  }
}

static void solve_equation(cigue_state* gui, double a, double b, double c) {
  if (isnan(a) || isnan(b) || isnan(c))
    cigue_label(gui, "Введите что-то вменяемое.");
  else if (!iszero(a))
    solve_sqr(gui, a, b, c);
  else if (!iszero(b))
    solve_linear(gui, b, c);
  else
    solve_const(gui, c);
}

void sqe_cigue_ui() {

  cigue_state* gui = cigue_new_state();
  cigue_tty_init();

  char* key = "\1";
  double a, b, c;
  char a_buf[21] = "1", b_buf[21] = "4", c_buf[21] = "3";
  int selected = 1;

  while (strcmp(key, "q")) {
    if (*key == '\0') {
      key = cigue_tty_nextkey();
      continue;
    }

    // Двигаемся вправо/влево по полям ввода.
    if (!strcmp(key, K_RIGHT) && selected < 2)
      ++selected;
      
    if (!strcmp(key, K_LEFT) && selected > 0)
      --selected;

    cigue_begin_column(gui, 1);
      logo(gui);
      cigue_begin_row(gui, 0);
        a = text_input(gui, a_buf, 20, selected == 0, key);
        cigue_label(gui, "x² + ");
        b = text_input(gui, b_buf, 20, selected == 1, key);
        cigue_label(gui, "x + ");
        c = text_input(gui, c_buf, 20, selected == 2, key);
        cigue_label(gui, " = 0");
      cigue_end(gui);
      // Разделителей нет - не хочу уходить в дебри алгоритмов 
      // layout-инга.
      cigue_begin_column(gui, 1);
        solve_equation(gui, a, b, c);
      cigue_end(gui);
    cigue_end(gui);

    cigue_tty_clear();
    cigue_render(0, 0, gui);
    key = cigue_tty_nextkey();
  }
}
