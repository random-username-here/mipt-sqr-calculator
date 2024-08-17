#include "cigue/memory.h"
#include "cigue/widgets.h"
#include "cigue/base.h"

typedef struct {
  int spacing;
} self_data;

static void layout_and_draw(cigue_state* s, cigue_widget* self) {
  self_data* data = (self_data*) self->widget_data;
  int pos = self->x;
  for (cigue_widget* child = self->first_child; child != NULL; child = child->next) {
    child->x = pos;
    // Здесь задано выравнивание по вертикали
    child->y = self->y + self->above_baseline - child->above_baseline;
    pos += child->width + data->spacing;
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

  int w = -data->spacing, above = 0, below = 0;
  for (cigue_widget* child = self->first_child; child != NULL; child = child->next) {
    w += child->width + data->spacing;
    if (child->above_baseline > above)
      above = child->above_baseline;
    if (child->height - child->above_baseline > below)
      below = child->height - child->above_baseline;
  }
  
  self->above_baseline = above;
  self->width = w;
  self->height = above + below;
}

void cigue_begin_row(cigue_state* s, int spacing) {
  cigue_widget* wgt = cigue_mem_alloc(s->buf, sizeof(cigue_widget));

  self_data* data = cigue_mem_alloc(s->buf, sizeof(self_data));
  data->spacing = spacing; 
  wgt->widget_data = data;

  wgt->compute_dimensions = &compute_size;
  wgt->layout_and_draw = &layout_and_draw;

  cigue_begin(s, wgt); 
}
