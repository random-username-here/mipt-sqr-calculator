//! 
//! Что-то вроде curses, только с современными фичами.
//!
//! Не использует terminfo, т.к. VT100 вы сегодня найдёте
//! разве что в музее. Все пользуются стандартными, xterm-
//! подобными терминалами. (не считая Windows-ятников)
//!
#pragma once

/// Какая-то точка на экране.
struct cigue_vec2 {
  int x, y;
};

typedef struct cigue_vec2 cigue_vec2;

/// Инициализируем терминал
void cigue_tty_init();

/// Завершаем псевдографический режим.
/// Эта функция автоматически вызывается при завершении программы,
/// так что вызывать её не обязательно (в отличии от `endscr()` из
/// curses).
void cigue_tty_deinit();

/// Получаем размеры терминала, сохраняем в dims.
/// TODO
// cigue_vec2 cigue_tty_get_size();

/// Выводим строку в заданном месте
void cigue_tty_puts(int x, int y, const char* text);

/// Просто выводим какую-то строку.
/// Сделано для вывода esc-кодов, для которых без разницы,
/// где они выведены.
void cigue_tty_puts_anywhere(const char* text);

/// Очищаем терминал.
void cigue_tty_clear();

/// Возвращает строку с последней нажатой клавишей.
/// Указатель указывает в статический массив, так что его чистить не нужно.
char* cigue_tty_nextkey();

/// Получаем один символ со ввода.
char cigue_tty_getch();
