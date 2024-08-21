#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "cigue/tty.h"
#include "cigue/base.h"
#include "cigue/widgets.h"
#include "sqe/cigue.h"

// TODO: const is better
double to_double(const char* buf); // TODO: extract into other file?

void logo(cigue_state* gui); // TODO: print_*

void handle_focus(int* selected, int num_inputs, const char* key);

double text_input(cigue_state* gui, char* buf, size_t maxlen,
                         bool focused, char* keypress);

////////////////////////////////////////////////////////////

double to_double(const char* buf) { // TODO: i see nothing bad in marking this as static, please conslult with previous mentor about the reason for such decision
  
  assert(buf);

  if (*buf == 0) // ничего не введено // TODO: it's better to use type-explicit notation '\0'
    return NAN;
  else {
    char* end = NULL;
    double res = strtod(buf, &end); 
    if (!end || *end || isnan(res))
      return NAN;
    return res;
  }
}

void logo(cigue_state* gui) {

  cigue_column(gui, 0) {
    cigue_label(gui, "┌─┐┌─┐ ┬─┐  ┌─┐┌─┐ ┬ ┬┌─┐┌┬┐┬┌─┐┌┐┌┌─┐"); // TODO: cigue_put_text?
    cigue_label(gui, "└─┐│─┼┐├┬┘  ├┤ │─┼┐│ │├─┤ │ ││ ││││└─┐");
    cigue_label(gui, "└─┘└─┘└┴└─  └─┘└─┘└└─┘┴ ┴ ┴ ┴└─┘┘└┘└─┘");
    cigue_label(gui, "");
    cigue_label(gui, CIGUE_E_GRAY "  [q]" CIGUE_E_RESET " Чтобы завершить программу");
    cigue_label(gui, CIGUE_E_GRAY "  [<-], [->]" CIGUE_E_RESET " Чтобы двигаться между полями ввода");
  }
}


// TODO: better name, maybe handle_input_key...
//
// TODO: maybe you can make a similar function handle_text_input
// that will just add a symbol when it's already known that user
// added one. Special keys (like left arrow or ^X can be handled
// or discarded before its called)
double text_input(cigue_state* gui, char* buf, size_t maxlen,
                         bool focused, char* keypress) { // TODO: alignment

  assert(gui);
  assert(buf);
  assert(keypress);

  // Обрабатываем ввод текста
  if (focused) {
    size_t len = strlen(buf);
    if (len > 0 && !strcmp(keypress, CIGUE_K_BACKSPACE)) // !strcmp(...) == 'x not equal y' but it's actually the opposite, I'd rather use strcmp(...) == 0
      buf[len-1] = 0; // TODO: стираем юникод.
    if (strlen(keypress) == 1 && len < maxlen && isprint(*keypress)) {
      buf[len] = *keypress;
      buf[len+1] = 0;
    }
  }

  double dbl = to_double(buf);

  // Риусуем UI
  cigue_begin_border(gui); // TODO: inconsistent
    cigue_labelf(gui, "%s%20s%s %s" CIGUE_E_RESET, (isnan(dbl) ? CIGUE_E_RED : ""),
        buf, (focused ? "█" : " "), (isnan(dbl) ? "x" : " "));
  cigue_end(gui);

  return dbl;
}

/// Увелчиваем или уменьшаем значение в пределах
/// `[0; num_inputs)` клавишами `K_LEFT`/`K_RIGHT`.
void handle_focus(int* selected, int num_inputs, const char* key) {

  assert(selected);
  assert(key);

  if (!strcmp(key, CIGUE_K_RIGHT) && *selected < num_inputs-1)
    ++*selected; 
  if (!strcmp(key, CIGUE_K_LEFT) && *selected > 0)
    --*selected;
}

void sqe_cigue_ui() {

  cigue_state* gui = cigue_new_state();
  cigue_tty_init();

  char _s[] = "\1"; // TODO: better name
  char* key = _s; // TODO: read about compound literals
  double a, b, c; // TODO: initialize variables
  char a_buf[21] = "1", b_buf[21] = "4", c_buf[21] = "3"; // TODO: weird buffer sizes
  // TODO: Variable Length Array (VLA), char buffer[size]; (in cpp we have alloca)
  int selected = 0; // TODO: a separate struct for "game" state?

  while (!key || strcmp(key, "q")) {
    if (*key == '\0') {
      key = cigue_tty_nextkey();
      continue;
    }

    handle_focus(&selected, 3, key);

    // Двигаемся вправо/влево по полям ввода.
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
          solve_result res = sqe_solve(a, b, c); // TODO: cigue.c name feels like library code, but it's not.
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
