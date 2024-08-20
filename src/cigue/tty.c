#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include "cigue/tty.h"

/// Эти переменные "глобальные",
/// т.к. я не намереваюсь заставлять пользователя передавать
/// данные в любой вызов cigue_tty_puts.
///
/// А если смотреть со стороны всей программы,
/// эти переменные не глобальные. Они статические,
/// то есть находятся только в этом небольшом файле.
///
static bool atexit_registered = false,
            tty_initted = false;
static char keybuf[10];
static struct termios original;

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

  tcgetattr(STDIN_FILENO, &original);
  
  struct termios new_opts = original;
  new_opts.c_lflag &= ~(ECHO | ICANON);
  new_opts.c_cc[VMIN] = 0;
  new_opts.c_cc[VTIME] = 0;
  
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);

  tty_initted = true;
}

void cigue_tty_deinit() {

  if (!tty_initted)
    return;
  tty_initted = false;
  
  printf("\x1b[?1049l"); // no alt mode
  printf("\x1b[?25h"); // cursor
  
  tcsetattr(STDIN_FILENO, TCSANOW, &original); // возвращаем терминал в изначальное состояние
}

void cigue_tty_puts_anywhere(const char* text) {

  assert(text && "Text to write must be != NULL");
  
  printf("%s", text);
  fflush(stdout);
}

void cigue_tty_puts(int x, int y, const char* text) {

  assert(text && "Text to write must be != NULL");
  
  printf("\x1b[%d;%dH%s", y+1, x+1, text);
  fflush(stdout);
}

void cigue_tty_clear() {

  printf("\x1b[2J");
  fflush(stdout);
}

char* cigue_tty_nextkey() {

  int kl = 0;
  keybuf[kl++] = cigue_tty_getch();
  
  if (keybuf[0] == '\x1b') { // escape-коды
    // Например "\x1b[A"
    keybuf[kl++] = cigue_tty_getch();
    keybuf[kl++] = cigue_tty_getch();
  } // TODO: UTF8, вроде русских букв
  
  keybuf[kl] = 0;
  
  return keybuf;
}

char cigue_tty_getch() {

  char res = 0;
  read(STDIN_FILENO, &res, 1);
  
  return res;
}
