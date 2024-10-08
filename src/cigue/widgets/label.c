#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "cigue/memory.h"
#include "cigue/widgets.h"
#include "cigue/base.h"
#include "cigue/tty.h"

#ifdef CIGUE_GL
#include "glcvs/font.h"
#endif

typedef struct {
  const char* str;
  // форматирование, ...
} self_data;

static void layout_and_draw(cigue_state* s, cigue_widget* label) {

  self_data* data = (self_data*) label->widget_data;

  #ifdef CIGUE_GL
  if (s->ctx) {
    s->ctx->fill_color = CIGUE_TEXT;
    glcvs_fill_text(s->ctx, (float) label->x, (float) (label->y + label->above_baseline), data->str);
  } else
  #endif
  cigue_tty_puts(label->x, label->y, data->str);
}

void cigue_external_label(cigue_state* s, const char* text) {

  assert(s != NULL && "Widget must be created in GUI. You passed gui = NULL.");

  cigue_widget* wgt = cigue_mem_new(s->buf, cigue_widget);
  self_data* data = cigue_mem_new(s->buf, self_data);

  data->str = text;
  wgt->widget_data = data;


  #ifdef CIGUE_GL
  if (s->ctx) {
    glcvs_text_mertics metrics = glcvs_get_text_metrics();
    wgt->height = (int) (metrics.ascent + metrics.descent);
    wgt->above_baseline = (int) metrics.ascent;
    wgt->width = (int) glcvs_text_width(s->ctx, text);
  }else {
  #endif

  wgt->height = 1;
  wgt->above_baseline = 1;
  // https://stackoverflow.com/questions/32936646/getting-the-string-length-on-utf-8-in-c
  wgt->width = 0;
  int in_esc = 0;
  for (const char* c = text; *c != '\0'; ++c) {
    // Не считаем коды для форматирования
    if (*c == '\x1b') 
      in_esc = 1;
    else if (in_esc && *c == 'm')
      in_esc = 0;
    else if (!in_esc)
      wgt->width += (*c & 0xC0) != 0x80 ? 1 : 0;
  }

  #ifdef CIGUE_GL
  }
  #endif

  wgt->compute_dimensions = NULL;
  wgt->layout_and_draw = &layout_and_draw;

  cigue_begin(s, wgt);
  cigue_end(s);
}

// Дальше разные обёртки.

void cigue_label(cigue_state *s, const char *text) {

  cigue_external_label(s, cigue_mem_save_str(s->buf, text));
}

void cigue_labelvf(cigue_state* s, const char* fmt, va_list args) {

  va_list clone;
  va_copy(clone, args);

  // Получаем длинну и создаём буффер нужной длинны
  size_t len = (size_t) vsnprintf(NULL, 0, fmt, clone);
  char* buf = (char*) cigue_mem_alloc(s->buf, len+1);
  
  // Печатаем в него
  vsprintf(buf, fmt, args); // TODO: pragma warning?
  
  // Выводим подпись
  cigue_external_label(s, buf);
}

void cigue_labelf(cigue_state *s, const char *fmt, ...) {

  va_list args;
  va_start(args, fmt);
  cigue_labelvf(s, fmt, args);
}
