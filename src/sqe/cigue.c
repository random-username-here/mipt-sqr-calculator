#include "sqe/cigue.h"
#include "cigue/tty.h"
#include "cigue/base.h"
#include "cigue/widgets.h"
#include "stdio.h"

void sqe_cigue_ui() {

  cigue_state* gui = cigue_new_state();

  cigue_begin_column(gui, 1);
    cigue_label(gui, "Hello world!");
    cigue_begin_row(gui, 3);
      cigue_label(gui, "Second label");
      cigue_label(gui, "Next label");
      cigue_label(gui, "More labels!");
    cigue_end(gui);
    cigue_label(gui, "Other label");
  cigue_end(gui);

  cigue_tty_init();
  cigue_render(0, 0, gui);

  char c;
  scanf("%c", &c);
}
