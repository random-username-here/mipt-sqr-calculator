#include "sqe/cigue.h"
#include "cigue/widgets.h"
#include "cigue/math-widgets.h"

static void paint_const_any_solution(cigue_state* gui, const solve_result* res) {
  cigue_label(gui, "0 = 0");
  cigue_label(gui, "⇒ " ESC_BOLD "x любое" ESC_RESET);
}

static void paint_const_none_solution(cigue_state* gui, const solve_result* res) {
  cigue_labelf(gui, "%lf = 0", res->c);
  cigue_label(gui, "такое невозможно, так что " ESC_BOLD "x = ∅" ESC_RESET);
}

static void paint_linear_solution(cigue_state* gui, const solve_result* res) {
  cigue_row(gui, 0) {
    cigue_label(gui, "x = - ");
    cigue_math_frac(gui) {
      cigue_label(gui, "c");
      cigue_label(gui, "b");
    }
    cigue_label(gui, " = - ");
    cigue_math_frac(gui) {
      cigue_labelf(gui, "%lf", res->c);
      cigue_labelf(gui, "%lf", res->b);
    }
    cigue_labelf(gui, " = " ESC_BOLD "%lf" ESC_RESET, res->x1);
  }
}

static void paint_D_calc(cigue_state* gui, const solve_result* res) {
  cigue_row(gui, 0) {
    cigue_label(gui, "D");
    cigue_column(gui, 0) {
      cigue_label(gui, " = b² - 4ac");
      cigue_labelf(gui, " = (%lf)² - 4 * %lf * %lf", res->b, res->a, res->c);
      cigue_labelf(gui, " = %lf", res->D);
    }
  }
}

static void paint_sqr_none_solution(cigue_state* gui, const solve_result* res) {
  paint_D_calc(gui, res);
  cigue_label(gui, "D < 0 ⇒ решений нет");
  cigue_label(gui, ESC_BOLD "x = ∅" ESC_RESET);
}

static void paint_sqr_one_solution(cigue_state* gui, const solve_result* res) {
  paint_D_calc(gui, res);
  cigue_label(gui, "D = 0 ⇒ решение одно");

  cigue_row(gui, 0) {
    cigue_label(gui, "x = ");
    cigue_math_frac(gui) {
      cigue_label(gui, "-b");
      cigue_label(gui, "2a");
    }
    cigue_label(gui, " = ");
    cigue_math_frac(gui) {
      cigue_labelf(gui, "%lf", -res->b);
      cigue_labelf(gui, "2 * %lf", res->a);
    }
    cigue_labelf(gui, " = \x1b[1m%lf\x1b[0m", -res->b / 2 / res->a);
  }
}

static void paint_sqr_two_solution(cigue_state* gui, const solve_result* res) {
  paint_D_calc(gui, res);
  cigue_label(gui, "D > 0 ⇒ решения два");

  // Гигантское вычисление x.
  cigue_row(gui, 0) {
    cigue_label(gui, "x");
    cigue_column(gui, 0) {
      // Первая строка - формула
      cigue_row(gui, 0) {
        cigue_label(gui, " = ");
        cigue_math_frac(gui) {
          cigue_row(gui, 0) {
            cigue_label(gui, "-b ± ");
            cigue_math_sqrt(gui) {
              cigue_label(gui, "D");
            }
          }
          cigue_label(gui, "2a");
        }
      }
      // Вторая строка - формула с значениями
      cigue_row(gui, 0) {
        cigue_label(gui, " = ");
        cigue_math_frac(gui) {
          cigue_row(gui, 0) {
            cigue_labelf(gui, "%lf ± ", -res->b);
            cigue_math_sqrt(gui) {
              cigue_labelf(gui, "%lf", res->D);
            }
          }
          cigue_labelf(gui, "2 * %lf", res->a);
        }
      }
      // Пропустим строку, т.к. выглядит слишком склеенно.
      cigue_label(gui, "");
      // Третья строка - ответы.
      cigue_labelf(gui, " = " ESC_BOLD "{%lf; %lf}" ESC_RESET, res->x1, res->x2);
    }
  }
}

typedef void (*solution_painter)(cigue_state* gui, const solve_result* res);

static const solution_painter solution_painters[] = {
  paint_const_any_solution,
  paint_const_none_solution,
  paint_linear_solution,
  paint_sqr_none_solution,
  paint_sqr_one_solution,
  paint_sqr_two_solution
};

void sqe_cigue_paint_solution(cigue_state* gui, const solve_result *res) {
  solution_painters[res->type](gui, res);
}
