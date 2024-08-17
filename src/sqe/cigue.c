#include "sqe/cigue.h"
#include "cigue/tty.h"
#include "stdio.h"

void sqe_cigue_ui() {
  cigue_tty_init();

  cigue_tty_puts(10,5, "Hello world!");
  int d;
  scanf("%d", &d);
}
