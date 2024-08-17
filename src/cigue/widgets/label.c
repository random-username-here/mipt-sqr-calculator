#include <string.h>
#include "cigue/memory.h"
#include "cigue/widgets.h"
#include "cigue/base.h"
#include "cigue/tty.h"

typedef struct {
  const char* str;
  // форматирование, ...
} self_data;

static void layout_and_draw(cigue_state* s, cigue_widget* label) {
  self_data* data = (self_data*) label->widget_data;
  cigue_tty_puts(label->x, label->y, data->str);
}

void cigue_external_label(cigue_state* s, const char* text) {
  cigue_widget* wgt = cigue_mem_alloc(s->buf, sizeof(cigue_widget));

  self_data* data = cigue_mem_alloc(s->buf, sizeof(self_data));
  data->str = text;
  wgt->widget_data = data;

  wgt->width = strlen(text);
  wgt->height = 1;
  wgt->above_baseline = 1;

  wgt->compute_dimensions = NULL;
  wgt->layout_and_draw = &layout_and_draw;

  cigue_begin(s, wgt);
  cigue_end(s);
}

// Дальше разные обёртки.

void cigue_label(cigue_state *s, const char *text) {
  cigue_external_label(s, cigue_mem_save_str(s->buf, text));
}
