#include "cigue/memory.h"
#include "cigue/widgets.h"
#include "cigue/base.h"

typedef struct {
  int spacing;
} row_data;

static void layout_column(cigue_state* s, cigue_widget* self) {
  row_data* data = (row_data*) self->widget_data;
  int pos = self->y;
  for (cigue_widget* child = self->first_child; child != NULL; child = child->next) {
    child->x = self->x;
    child->y = pos;
    pos += child->height + data->spacing;
  }
}

static void compute_size(cigue_state* s, cigue_widget* self) {
  row_data* data = (row_data*) self->widget_data;

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
  cigue_widget* wgt = cigue_mem_alloc(s->buf, sizeof(cigue_widget));

  row_data* data = cigue_mem_alloc(s->buf, sizeof(row_data));
  data->spacing = spacing; 
  wgt->widget_data = data;

  wgt->compute_dimensions = &compute_size;
  wgt->layout_and_draw = &layout_column;

  cigue_begin(s, wgt); 
}
