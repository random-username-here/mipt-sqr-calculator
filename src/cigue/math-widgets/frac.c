#include "cigue/memory.h"
#include "cigue/math-widgets.h"
#include "cigue/tty.h"
#include <assert.h>

typedef struct {
} self_data;

static void layout_and_draw(cigue_state* s, cigue_widget* self) {
  //self_data* data = (self_data*) self->widget_data;

  self->first_child->x = self->x + self->width/2 - self->first_child->width/2;
  self->first_child->y = self->y;
  self->first_child->next->x = self->x + self->width/2 - self->first_child->next->width/2;
  self->first_child->next->y = self->y + self->first_child->height + 1;

  for (int i = 0; i < self->width; ++i)
    cigue_tty_puts(self->x + i, self->y + self->first_child->height, "─");
}

static inline int max(int a, int b) {
  return a > b ? a : b;
}

static void compute_size(cigue_state* s, cigue_widget* self) {
  //self_data* data = (self_data*) self->widget_data;

  assert(self->first_child && "fraction requires numerator");
  assert(self->first_child->next && "fraction requires denominator");
  assert(self->first_child->next->next == NULL && "fraction requres two sub-elements");

  self->width = max(self->first_child->width, self->first_child->next->width) + 2;
  self->height = self->first_child->height + self->first_child->next->height + 1;
  self->above_baseline = self->first_child->height + 1;
}

void cigue_math_begin_frac(cigue_state* s) {
  cigue_widget* wgt = cigue_mem_new(s->buf, cigue_widget);
  self_data* data = cigue_mem_new(s->buf, self_data);

  wgt->widget_data = data;

  wgt->compute_dimensions = &compute_size;
  wgt->layout_and_draw = &layout_and_draw;

  cigue_begin(s, wgt); 
}
