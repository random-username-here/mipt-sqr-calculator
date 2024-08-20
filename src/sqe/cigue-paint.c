#include <assert.h>
#include "cigue/base.h"
#include "sqe/cigue.h"
#include "cigue/widgets.h"
#include "cigue/math-widgets.h"
#include "sqe/solve.h"

void paint_const_any_solution(cigue_state* gui, const solve_result* res);

void paint_const_none_solution(cigue_state* gui, const solve_result* res);

void paint_linear_solution(cigue_state* gui, const solve_result* res);

void paint_D_calc(cigue_state* gui, const solve_result* res);

void paint_sqr_none_solution(cigue_state* gui, const solve_result* res);

void paint_sqr_one_solution(cigue_state* gui, const solve_result* res);

void paint_sqr_two_solution(cigue_state* gui, const solve_result* res);

void _sqr_twosolution_formula(cigue_state* gui);

void _sqr_twosolution_formula_computation(cigue_state* gui, const solve_result* res);

////////////////////////////////////////////////////////////

void paint_const_any_solution(cigue_state* gui, const solve_result* res) {

  cigue_label(gui, "0 = 0");
  cigue_label(gui, "⇒ " ESC_BOLD "x любое" ESC_RESET);
}

void paint_const_none_solution(cigue_state* gui, const solve_result* res) {

  cigue_labelf(gui, "%lf = 0", res->c);
  cigue_label(gui, "такое невозможно, так что " ESC_BOLD "x = ∅" ESC_RESET);
}

void paint_linear_solution(cigue_state* gui, const solve_result* res) {

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

void paint_D_calc(cigue_state* gui, const solve_result* res) {

  cigue_row(gui, 0) {
    cigue_label(gui, "D");
    cigue_column(gui, 0) {
      cigue_label(gui, " = b² - 4ac");
      cigue_labelf(gui, " = (%lf)² - 4 * %lf * %lf", res->b, res->a, res->c);
      cigue_labelf(gui, " = %lf", res->D);
    }
  }
}

void paint_sqr_none_solution(cigue_state* gui, const solve_result* res) {

  paint_D_calc(gui, res);
  cigue_label(gui, "D < 0 ⇒ решений нет");
  cigue_label(gui, ESC_BOLD "x = ∅" ESC_RESET);
}

void paint_sqr_one_solution(cigue_state* gui, const solve_result* res) {

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

void _sqr_twosolution_formula(cigue_state* gui) {

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
}

void _sqr_twosolution_formula_computation(cigue_state* gui, const solve_result* res) {

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
}

void paint_sqr_two_solution(cigue_state* gui, const solve_result* res) {

  paint_D_calc(gui, res);
  cigue_label(gui, "D > 0 ⇒ решения два");

  // Гигантское вычисление x.
  cigue_row(gui, 0) {
    cigue_label(gui, "x");
    cigue_column(gui, 0) {
      // Первая строка - формула
      _sqr_twosolution_formula(gui); 
      // Вторая строка - формула с значениями
      _sqr_twosolution_formula_computation(gui, res);
      // Пропустим строку, т.к. выглядит слишком склеенно.
      cigue_label(gui, "");
      // Третья строка - ответы.
      cigue_labelf(gui, " = " ESC_BOLD "{%lf; %lf}" ESC_RESET, res->x1, res->x2);
    }
  }
}

void sqe_cigue_paint_solution(cigue_state* gui, const solve_result *res) {

  switch(res->type) {
    case RESULT_CONST_ANY:  paint_const_any_solution(gui, res);  break;
    case RESULT_CONST_NONE: paint_const_none_solution(gui, res); break;
    case RESULT_LINEAR:     paint_linear_solution(gui, res);     break;
    case RESULT_SQR_NONE:   paint_sqr_none_solution(gui, res);   break;
    case RESULT_SQR_ONE:    paint_sqr_one_solution(gui, res);    break;
    case RESULT_SQR_TWO:    paint_sqr_two_solution(gui, res);    break;
    default:                assert(false);                       break;
  }
}
