//! 
//! Основа библиотеки. Здесь написана структура виджетов,
//! состояние и функции, использующиеся для отрисовки всего дерева.
//!
//! Все вещи, которые нужны только один кадр (в т.ч. сами виджеты)
//! создаются в буффере.
//!

#pragma once
#include "cigue/memory.h"

struct cigue_state;

struct cigue_widget {
  /// Следующий виджет в списке детей какого-то
  /// родительского виджета.
  struct cigue_widget* next;

  /// Первый ребёнок этого виджета.
  /// Следующие за ним можно добыть как `first_child->next->next...`.
  struct cigue_widget* first_child;

  /// Высота виджета над строкой.
  /// Если виджет - одна строка текста,
  /// то он весь над строкой.
  int above_baseline;

  /// Размеры этого виджета. Ни больше, ни меньше.
  int width, height;

  /// Местоположение этого виджета на экране.
  int x, y;

  /// Данная функция считает ширину и высоту этого виджета,
  /// уже зная всё содержимое.
  ///
  /// Можно задать `NULL`, тогда `width` и `height` останется таким же,
  /// с каким этот виджет был создан.
  void (*compute_dimensions)(struct cigue_state* state, struct cigue_widget* self);

  /// Считаем позиции детей, зная свою.
  /// Затем рисуем этот виджет.
  /// Детей не нужно рисовать, их отрисует `cigue_render()`.
  ///
  /// Это последний вызов кадра, так что если в `cigue_state.buf`
  /// было что-то, что нужно чистить, то это нужно сделать здесь.
  void (*layout_and_draw)(struct cigue_state* state, struct cigue_widget* self);

  /// Данные этого виджета.
  /// Ими распоражаются 
  void* widget_data;
};

typedef struct cigue_widget cigue_widget;

struct _cigue_widget_stack {
  struct _cigue_widget_stack* parent;
  cigue_widget* wgt;
  cigue_widget** new_child_place;
};

typedef struct _cigue_widget_stack _cigue_widget_stack;

/// Состояние всего GUI.
struct cigue_state {
  /// Буффер, из которого аллоцируется память на виджеты и
  /// их внутренние свойства.
  cigue_mem_buffer* buf;

  /// Первый виджет.
  cigue_widget* _first;

  /// Стек, используемый при `begin/end_widget()`
  /// Не трогать.
  /// Аллоцируется в буффере
  _cigue_widget_stack* _stack_top;
};

typedef struct cigue_state cigue_state;

/// Создаём объект, в котором данные, которые хранятся дольше одного кадра.
cigue_state* cigue_new_state();

/// Удаляем этот объект.
void cigue_free_state(cigue_state* state);

/// Мы построили дерево виджетов, теперь нужно их отрисовать.
/// После этого состояние подготавливается к новому кадру.
void cigue_render(int x, int y, cigue_state* state);

/// Начали новый виджет - как открыли HTML-ный тег.
/// Сам виджет должен быть аллоцирован в буффере.
void cigue_begin(cigue_state* state, cigue_widget* widget);

/// Закрыли этот виджет.
void cigue_end(cigue_state* state);
