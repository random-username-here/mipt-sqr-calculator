//!
//! Парсер аргументов.
//!

/// Проверяем есть ли аргумент с таким названием
///
/// @param [in] argc - Количество аргументов программы
/// @param [in] argv - Значения аргументов
/// @param [in] name - Название аргумента, который ищем
/// @param [out] output - Если аргумент есть, то туда 
bool sqe_find_arg(int argc, const char* argv, const char* name, char** output);

