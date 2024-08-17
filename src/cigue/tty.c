#include "cigue/tty.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

static bool atexit_registered = false,
            tty_initted = false;

void cigue_tty_init() {
  if (tty_initted)
    return;

  if (!atexit_registered) {
    atexit(cigue_tty_deinit);
    atexit_registered = true;
  }

  printf("\x1b[?1049h"); // alt mode
  printf("\x1b[?25l"); // no cursor
  cigue_tty_clear();
  tty_initted = true;
}

void cigue_tty_deinit() {
  if (!tty_initted)
    return;
  tty_initted = false;
  printf("\x1b[?1049l"); // no alt mode
  printf("\x1b[?25h"); // cursor
}

void cigue_tty_puts(int x, int y, const char* text) {
  printf("\x1b[%d;%dH%s", y+1, x+1, text);
  fflush(stdout);
}

void cigue_tty_clear() {
  printf("\x1b[2J");
  fflush(stdout);
}
