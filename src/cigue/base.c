#include "cigue/base.h"
#include "cigue/memory.h"
#include <stdlib.h>
#include <assert.h>

cigue_state* cigue_new_state() {
  cigue_state* st = (cigue_state*) malloc(sizeof(cigue_state));
  st->buf = cigue_mem_new_buffer(1024*64);
  st->_first = NULL;
  st->_stack_top = NULL;
  return st;
}

void cigue_free_state(cigue_state* state) {
  cigue_mem_free_buffer(state->buf);
  free(state);
}

static void recurse_compute_dims(cigue_state* st, cigue_widget* wgt) {
  if (wgt->first_child)
    recurse_compute_dims(st, wgt->first_child);
  if (wgt->compute_dimensions)
    wgt->compute_dimensions(st, wgt);
  if (wgt->next)
    recurse_compute_dims(st, wgt->next);
}

static void recurse_render(cigue_state* st, cigue_widget* wgt) {
  if (wgt->layout_and_draw)
    wgt->layout_and_draw(st, wgt);
  if (wgt->first_child)
    recurse_render(st, wgt->first_child);
  if (wgt->next)
    recurse_render(st, wgt->next);
}


void cigue_render(int x, int y, cigue_state* state) {
  // Сам рендер
  recurse_compute_dims(state, state->_first);
  state->_first->x = x;
  state->_first->y = y;
  recurse_render(state, state->_first);

  // Подготовка к новому кадру
  state->_first = NULL;
  state->_stack_top = NULL;
  cigue_mem_new_frame(state->buf);
}

void cigue_begin(cigue_state* state, cigue_widget* widget) {
  _cigue_widget_stack* stack_item = cigue_mem_new(state->buf, _cigue_widget_stack);
  if (state->_first == NULL) {
    // Первый виджет
    state->_first = widget;
    stack_item->parent = NULL;
  } else if (state->_stack_top == NULL) {
    // Не первый виджет, но стек пустой.
    // Получили второй корневой виджет.
    assert(0 && "There must be only one root widget");
  } else {
    // Просто какой-то виджет.
    *state->_stack_top->new_child_place = widget;
    stack_item->parent = state->_stack_top;
  }
  stack_item->wgt = widget;
  stack_item->new_child_place = &widget->first_child;
  state->_stack_top = stack_item;
}

void cigue_end(cigue_state* state) {
  assert(state->_stack_top != NULL && "There should be some widget to end");
  _cigue_widget_stack* item = state->_stack_top;
  state->_stack_top = state->_stack_top->parent;
  if (!state->_stack_top) return; // закрыли корневой
  state->_stack_top->new_child_place = &item->wgt->next;
  // `item` остаётся в буффере, хотя он уже не нужен.
  // TODO: переиспользовать их
}
