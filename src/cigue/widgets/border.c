#include "cigue/memory.h"
#include "cigue/widgets.h"
#include "cigue/base.h"
#include "cigue/tty.h"
#include <assert.h>

// Данный код служит иллюстрацией того, зачем нужны шаблоны.

typedef struct {
} self_data;

#define ESC_GRAY "\x1b[90m"
#define ESC_RESET "\x1b[0m"

static void layout_and_draw(cigue_state* s, cigue_widget* self) {

  //self_data* data = (self_data*) self->widget_data;
  self->first_child->x = self->x + 1;
  self->first_child->y = self->y + 1;

  cigue_tty_puts_anywhere(ESC_GRAY); // серый цвет

  // Ненавижу писать эту часть...
  cigue_tty_puts(self->x, self->y, "┌");
  cigue_tty_puts(self->x + self->width - 1, self->y, "┐");
  cigue_tty_puts(self->x, self->y + self->height - 1, "└");
  cigue_tty_puts(self->x + self->width - 1, self->y + self->height - 1, "┘");

  for (int i = 1; i < self->width - 1; ++i) {
    cigue_tty_puts(self->x + i, self->y, "─");
    cigue_tty_puts(self->x + i, self->y + self->height - 1, "─");
  }
  
  for (int i = 1; i < self->height - 1; ++i) {
    cigue_tty_puts(self->x, self->y + i, "│");
    cigue_tty_puts(self->x + self->width - 1, self->y + i, "│");
  }

  cigue_tty_puts_anywhere(ESC_RESET);
}

static void compute_size(cigue_state* s, cigue_widget* self) {

  assert(s != NULL && "Widget must be created in GUI. You passed gui = NULL.");
  assert(self->first_child && "cigue_border() must have a child");
  assert(self->first_child->next == NULL && "cigue_border() must have exactly one child");


  //self_data* data = (self_data*) self->widget_data;

  self->width = self->first_child->width + 2;
  self->height = self->first_child->height + 2;
  self->above_baseline = self->first_child->above_baseline + 1;
}

void cigue_begin_border(cigue_state* s) {

  cigue_widget* wgt = cigue_mem_new(s->buf, cigue_widget);
  self_data* data = cigue_mem_new(s->buf, self_data);

  wgt->widget_data = data;

  wgt->compute_dimensions = &compute_size;
  wgt->layout_and_draw = &layout_and_draw;

  cigue_begin(s, wgt); 
}
