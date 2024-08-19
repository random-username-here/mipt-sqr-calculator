//! 
//! Аллокатор, используемый виждетами вроде cigue_label
//! для хранения данных от их создания, во время рассчёта
//! позиций, до отрисовки.
//!
//! Освобождать полученную таким образом память не нужно,
//! т.к. она выделяется в буффере, который будет перезаписан
//! на следующий кадр.
//!

#pragma once
#include <stddef.h>
#include <stdalign.h>

/// Сама структура буффера.
typedef struct {
  /// Память, которую раздают
  void* memory;

  /// Сколько всего памяти.
  size_t avail_size;

  /// Всё, что до `memory + alloc_point` роздано.
  /// Всё что после - нет.
  size_t alloc_point;
} cigue_mem_buffer;


/// Создаёт новый буффер, из которого можно аллоцировать с помощью
/// `cigue_mem_alloc/save/save_str`.
cigue_mem_buffer* cigue_mem_new_buffer(size_t initial_size);

/// Освобождает буффер.
void cigue_mem_free_buffer(cigue_mem_buffer* buf);

/// Начался новый кадр, выкидываем всё из буффера.
void cigue_mem_new_frame(cigue_mem_buffer* buf);

/// Аллоцируем `size` байт из буффера
void* cigue_mem_alloc(cigue_mem_buffer* buf, size_t size);

/// Аллоцируем `size` байт, выровненные как сказано.
/// Интересно, какой архитектуре это нужно?
void* cigue_mem_alloc_aligned(cigue_mem_buffer* buf, size_t size, size_t align);

/// Сохраняет объект заданного размера в буффер.
void* cigue_mem_save(cigue_mem_buffer* buf, const void* object, size_t size);

/// Сохраняет строку, заканчивающуюся на `\0` в буффер.
char* cigue_mem_save_str(cigue_mem_buffer* buf, const char* str);

/// Проверяет, что данный указатель находится в
/// данном буффуре. Если нет - роняет программу.
void cigue_assert_buf_memory(cigue_mem_buffer* buf, const void* mem);

/// Создаём объект заданного типа, выровнянный как нужно.
#define cigue_mem_new(buf, type) ((type*) cigue_mem_alloc_aligned(buf, sizeof(type), alignof(type)))
