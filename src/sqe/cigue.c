#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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
    if (*end || isnan(res))
      return NAN;
    return res;
  }}

const char K_BACKSPACE[] = "\x7f";
const char K_LEFT[] = "\x1b[D";
const char K_RIGHT[] = "\x1b[C";

static void logo(cigue_state* gui) {
  cigue_column(gui, 0) {
    cigue_label(gui, "┌─┐┌─┐ ┬─┐  ┌─┐┌─┐ ┬ ┬┌─┐┌┬┐┬┌─┐┌┐┌┌─┐");
    cigue_label(gui, "└─┐│─┼┐├┬┘  ├┤ │─┼┐│ │├─┤ │ ││ ││││└─┐");
    cigue_label(gui, "└─┘└─┘└┴└─  └─┘└─┘└└─┘┴ ┴ ┴ ┴└─┘┘└┘└─┘");
    cigue_label(gui, "");
    cigue_label(gui, "\x1b[90m  [q]\x1b[0m Чтобы завершить программу");
    cigue_label(gui, "\x1b[90m  [<-], [->]\x1b[0m Чтобы двигаться между полями ввода");
  }
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
    cigue_labelf(gui, "%s%20s%s %s" ESC_RESET, (isnan(dbl) ? ESC_RED : ""),
        buf, (focused ? "█" : " "), (isnan(dbl) ? "x" : " "));
  cigue_end(gui);

  return dbl;
}

void sqe_cigue_ui() {
  cigue_state* gui = cigue_new_state();
  cigue_tty_init();


  char _s[] = "\1";
  char* key = _s;
  double a, b, c;
  char a_buf[21] = "1", b_buf[21] = "4", c_buf[21] = "3";
  int selected = 1;

  while (!key || strcmp(key, "q")) {
    if (*key == '\0') {
      key = cigue_tty_nextkey();
      continue;
    }

    // Двигаемся вправо/влево по полям ввода.
    if (!strcmp(key, K_RIGHT) && selected < 2)
      ++selected;
      
    if (!strcmp(key, K_LEFT) && selected > 0)
      --selected;

    cigue_column(gui, 1) {
      logo(gui);

      cigue_row(gui, 0) {
        a = text_input(gui, a_buf, 20, selected == 0, key);
        cigue_label(gui, "x² + ");
        b = text_input(gui, b_buf, 20, selected == 1, key);
        cigue_label(gui, "x + ");
        c = text_input(gui, c_buf, 20, selected == 2, key);
        cigue_label(gui, " = 0");
      }
      // Разделителей нет - не хочу уходить в дебри алгоритмов 
      // layout-инга.
      cigue_column(gui, 1) {
        if (isnan(a) || isnan(b) || isnan(c))
          cigue_label(gui, "Введите что-то вменяемое.");
        else {
          solve_result res = sqe_solve(a, b, c);
          sqe_cigue_paint_solution(gui, &res);
        }
      }
    }

    cigue_tty_clear();
    cigue_render(0, 0, gui);

    key = cigue_tty_nextkey();
  }
  cigue_free_state(gui);
}
