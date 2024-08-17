#include <string.h>
#include "cigue/memory.h"
#include "cigue/widgets.h"
#include "cigue/base.h"
#include "cigue/tty.h"

typedef struct {
  const char* str;
  // форматирование, ...
} label_data;

static void draw_label(cigue_state* s, cigue_widget* label) {
  label_data* data = (label_data*) label->widget_data;
  cigue_tty_puts(label->x, label->y, data->str);
}

void cigue_external_label(cigue_state* s, const char* text) {
  cigue_widget* wgt = cigue_mem_alloc(s->buf, sizeof(cigue_widget));

  label_data* data = cigue_mem_alloc(s->buf, sizeof(label_data));
  data->str = text;
  wgt->widget_data = data;

  wgt->width = strlen(text);
  wgt->height = 1;
  wgt->above_baseline = 1;

  wgt->compute_dimensions = NULL;
  wgt->layout_and_draw = &draw_label;

  cigue_begin(s, wgt);
  cigue_end(s);
}

// Дальше разные обёртки.

void cigue_label(cigue_state *s, const char *text) {
  cigue_external_label(s, cigue_mem_save_str(s->buf, text));
}
