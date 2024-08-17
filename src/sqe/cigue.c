#include "sqe/cigue.h"
#include "cigue/tty.h"
#include "cigue/base.h"
#include "cigue/widgets.h"
#include "stdio.h"

void sqe_cigue_ui() {
  cigue_tty_init();

  cigue_state* gui = cigue_new_state();

  cigue_begin_column(gui, 1);
    cigue_label(gui, "Hello world!");
    cigue_label(gui, "Second label");
    cigue_label(gui, "Other label");
  cigue_end(gui);

  cigue_render(0, 0, gui);

  int d;
  scanf("%d", &d);
}
