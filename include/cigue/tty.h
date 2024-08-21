/*! 
 * @file
 * @brief Что-то вроде curses, только с современными фичами.
 *
 * Не использует terminfo, т.к. VT100 вы сегодня найдёте
 * разве что в музее. Все пользуются стандартными, xterm-
 * подобными терминалами. (не считая Windows-ятников)
*/
#pragma once

/// @{
/// Escape-коды для покраски текста в терминале.
/// Это строковые макросы, так что их можно
/// склеивать со строковыми константами
#define CIGUE_E_GRAY      "\x1b[90m"
#define CIGUE_E_RED       "\x1b[91m"
#define CIGUE_E_GREEN     "\x1b[92m"
#define CIGUE_E_YELLOW    "\x1b[93m"
#define CIGUE_E_BLUE      "\x1b[94m"
#define CIGUE_E_MAGENTA   "\x1b[95m"
#define CIGUE_E_CYAN      "\x1b[96m"
#define CIGUE_E_WHITE     "\x1b[97m"
/// @}


#define CIGUE_E_RESET     "\x1b[0m"
#define CIGUE_E_BOLD      "\x1b[1m"


#define CIGUE_K_BACKSPACE "\x7f"
#define CIGUE_K_LEFT      "\x1b[D"
#define CIGUE_K_RIGHT     "\x1b[C"


/// Инициализируем терминал
void cigue_tty_init();

/// Завершаем псевдографический режим.
/// Эта функция автоматически вызывается при завершении программы,
/// так что вызывать её не обязательно (в отличии от `endscr()` из
/// curses).
void cigue_tty_deinit(); // TODO: stop? finish? ...

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
