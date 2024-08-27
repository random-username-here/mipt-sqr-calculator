#include "cigue/base.h"
#include "cigue/memory.h"
#include "cigue/math-widgets.h"
#include "cigue/tty.h"
#include <assert.h>

#ifdef CIGUE_GL
#include "glcvs/font.h"
#include "glcvs/drawing.h"
#endif

typedef struct {
} self_data;

#define FRAC_XSPACE 10
#define FRAC_LINE_SPACE 4
#define RULE_H 1

typedef struct {
  int xspace; // Distance line extends beyoung left/right end of contents
  int line_height; // Height of rule line
  int line_space;  // Space around rule line
  int frac_line; // Ascent at which rule line is located
} frac_layout;

static frac_layout get_layout(cigue_state* s) {
  #ifdef CIGUE_GL
  if (s->ctx) {
    return (frac_layout) {
      .xspace = FRAC_XSPACE,
      .line_height = RULE_H,
      .line_space = FRAC_LINE_SPACE,
      .frac_line = (int) glcvs_get_text_metrics().fraction_line
    };
  } else {
  #endif
    return (frac_layout) {
      .xspace = 1,
      .line_height = 1,
      .line_space = 0,
      .frac_line = 0
    };
  #ifdef CIGUE_GL
  }
  #endif
}

static void layout_and_draw(cigue_state* s, cigue_widget* self) {

  //self_data* data = (self_data*) self->widget_data;

  // TODO: designated initializers?
  // *self = (struct type){ ... };

  frac_layout lyt = get_layout(s);

  self->first_child->x = self->x + self->width/2 - self->first_child->width/2;
  self->first_child->y = self->y;
  self->first_child->next->x = self->x + self->width/2 - self->first_child->next->width/2;
  self->first_child->next->y = self->y + self->first_child->height + lyt.line_space*2 + lyt.line_height;

  #ifdef CIGUE_GL
  if (s->ctx) {
    s->ctx->fill_color = CIGUE_TEXT;
    glcvs_fill_rect(s->ctx, self->x, self->y + self->first_child->height + lyt.line_space, self->width, lyt.line_height);
  } else {
  #endif

  for (int i = 0; i < self->width; ++i)
    cigue_tty_puts(self->x + i, self->y + self->first_child->height, "─");

  #ifdef CIGUE_GL
  }
  #endif
}


static inline int max(int a, int b) {

  return a > b ? a : b;
}

static void compute_size(cigue_state* s, cigue_widget* self) {

  //self_data* data = (self_data*) self->widget_data;

  assert(self->first_child && "fraction requires numerator");
  assert(self->first_child->next && "fraction requires denominator");
  assert(self->first_child->next->next == NULL && "fraction requres two sub-elements");

  frac_layout lyt = get_layout(s);

  self->width = max(self->first_child->width, self->first_child->next->width) + lyt.xspace*2; // TODO: padding constant?
  self->height = self->first_child->height + self->first_child->next->height + lyt.line_height + lyt.line_space*2;

  // NOTE: fraction line is aligned with bottom on mathematical baseline.
  //       It should be middle on baseline, but it is not that important for now.

  self->above_baseline = self->first_child->height + lyt.frac_line + lyt.line_height + lyt.line_space;
}

void cigue_math_begin_frac(cigue_state* s) {

  assert(s != NULL && "Widget must be created in GUI. You passed gui = NULL.");

  cigue_widget* wgt = cigue_mem_new(s->buf, cigue_widget);
  self_data* data = cigue_mem_new(s->buf, self_data);

  wgt->widget_data = data;

  wgt->compute_dimensions = &compute_size;
  wgt->layout_and_draw = &layout_and_draw;

  cigue_begin(s, wgt); 
}
