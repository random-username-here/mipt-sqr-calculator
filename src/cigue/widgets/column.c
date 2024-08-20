#include "cigue/memory.h"
#include "cigue/widgets.h"
#include "cigue/base.h"
#include <assert.h>

typedef struct {
  int spacing;
} self_data;

static void layout_and_draw(cigue_state* s, cigue_widget* self) {

  self_data* data = (self_data*) self->widget_data;
  int pos = self->y;
  for (cigue_widget* child = self->first_child; child != NULL; child = child->next) {
    child->x = self->x;
    child->y = pos;
    pos += child->height + data->spacing;
  }
}

static void compute_size(cigue_state* s, cigue_widget* self) {

  self_data* data = (self_data*) self->widget_data;

  if (self->first_child == NULL) {
    // Пустая колонка
    self->height = 0;
    self->width = 0;
    self->above_baseline = 0;
    return;
  }

  int h = -data->spacing, w = 0;
  for (cigue_widget* child = self->first_child; child != NULL; child = child->next) {
    h += child->height + data->spacing;
    if (w < child->width)
      w = child->width;
  }
  
  self->above_baseline = self->first_child->above_baseline;
  self->width = w;
  self->height = h;
}

void cigue_begin_column(cigue_state* s, int spacing) {

  assert(s != NULL && "Widget must be created in GUI. You passed gui = NULL.");
  cigue_widget* wgt = cigue_mem_new(s->buf, cigue_widget);
  self_data* data = cigue_mem_new(s->buf, self_data);

  data->spacing = spacing; 
  wgt->widget_data = data;

  wgt->compute_dimensions = &compute_size;
  wgt->layout_and_draw = &layout_and_draw;

  cigue_begin(s, wgt); 
}
