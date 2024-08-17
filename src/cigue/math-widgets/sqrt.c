#include "cigue/memory.h"
#include "cigue/math-widgets.h"
#include "cigue/tty.h"
#include <assert.h>

// ┌─┐
// ⎷x

//  ┌─┐
// \│x
//
// Так лучше.

typedef struct {
} self_data;

static void layout_and_draw(cigue_state* s, cigue_widget* self) {
  self_data* data = (self_data*) self->widget_data;
  self->first_child->x = self->x + 2;
  self->first_child->y = self->y + 1;

  cigue_tty_puts(self->x+1, self->y, "┌");
  cigue_tty_puts(self->x + self->width - 1, self->y, "┐");
  cigue_tty_puts(self->x, self->y + self->height - 1, "\\");

  for (int i = 2; i < self->width - 1; ++i)
    cigue_tty_puts(self->x + i, self->y, "─");
  
  for (int i = 1; i < self->height; ++i)
    cigue_tty_puts(self->x+1, self->y + i, "│");
}

static void compute_size(cigue_state* s, cigue_widget* self) {
  self_data* data = (self_data*) self->widget_data;

  assert(self->first_child && "Square root widget must have a child");
  assert(self->first_child->next == NULL && "Square root widget must have exactly one child");

  self->width = self->first_child->width + 3;
  self->height = self->first_child->height + 1;
  self->above_baseline = self->first_child->above_baseline + 1;
}

void cigue_math_begin_sqrt(cigue_state *s) {
  cigue_widget* wgt = cigue_mem_alloc(s->buf, sizeof(cigue_widget));

  self_data* data = cigue_mem_alloc(s->buf, sizeof(self_data));
  wgt->widget_data = data;

  wgt->compute_dimensions = &compute_size;
  wgt->layout_and_draw = &layout_and_draw;

  cigue_begin(s, wgt); 
}
