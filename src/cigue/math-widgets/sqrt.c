#include "cigue/base.h"
#include "cigue/memory.h"
#include "cigue/math-widgets.h"
#include "cigue/tty.h"
#include "glcvs/buffers.h"
#include "glcvs/context.h"
#include <assert.h>

#ifdef CIGUE_GL
#include "glcvs/drawing.h"
#endif

// ┌─┐
// ⎷x

//  ┌─┐
// \│x
//
// Так лучше.

typedef struct {
} self_data;

typedef struct {
  int pref_size; // Width before text
  int suf_size;  // Width after text
  int top_size; // Height above text
} sqrt_layout;

static sqrt_layout get_layout(cigue_state* s) {
  #ifdef CIGUE_GL
  if (s->ctx) {
    return (sqrt_layout) {
      .pref_size = 15,
      .suf_size = 5,
      .top_size = 5
    };
  } else {
  #endif
    return (sqrt_layout) {
      .pref_size = 2,
      .suf_size = 1,
      .top_size = 1
    };
  #ifdef CIGUE_GL
  }
  #endif
}

static void layout_and_draw(cigue_state* s, cigue_widget* self) {

  //self_data* data = (self_data*) self->widget_data;
  
  sqrt_layout lyt = get_layout(s);

  self->first_child->x = self->x + lyt.pref_size;
  self->first_child->y = self->y + lyt.top_size;


  #ifdef CIGUE_GL
  if (s->ctx) {
    s->ctx->fill_color = CIGUE_TEXT;
    glcvs_fill_rect(s->ctx, self->x + lyt.pref_size - 5, self->y + 1, self->width - lyt.pref_size + 5, 1);
    glcvs_fill_rect(s->ctx, self->x + lyt.pref_size - 5, self->y + 1, 1, self->height - 1); 

    //             o .... o .............. o
    //             :      :                :
    //  (a) -- (b) o .... : .............. o
    //    \      \ :      :
    //     \      \:      :
    //      \      \      :
    //       \     :\     :
    //        \    : \    :
    //         \   :  \   :
    //          \  :   \  :
    //           \ :    \ :
    //            \:     \:
    //            (c) -- (d)
    // ----------->|<---->|
    //  pref_size      1

    glcvs_buf_begin_new_shape(s->ctx);
    unsigned a = glcvs_buf_add_vertex(s->ctx, (_glcvs_vertex) { .pos = (glcvs_point){ self->x + 5, self->y + self->height - 10 } });
    unsigned b = glcvs_buf_add_vertex(s->ctx, (_glcvs_vertex) { .pos = (glcvs_point){ self->x + 7, self->y + self->height - 11 } });
    unsigned c = glcvs_buf_add_vertex(s->ctx, (_glcvs_vertex) { .pos = (glcvs_point){ self->x + lyt.pref_size - 5, self->y + self->height } });
    unsigned d = glcvs_buf_add_vertex(s->ctx, (_glcvs_vertex) { .pos = (glcvs_point){ self->x + lyt.pref_size - 5 + 1, self->y + self->height } });

    glcvs_buf_add_triangle(s->ctx, a, b, c);
    glcvs_buf_add_triangle(s->ctx, b, c, d);

    glcvs_buf_draw(s->ctx);

    // TODO: more drawing
  } else {
  #endif
    cigue_tty_puts(self->x+1, self->y, "┌");
    cigue_tty_puts(self->x + self->width - 1, self->y, "┐");
    cigue_tty_puts(self->x, self->y + self->height - 1, "\\");

    for (int i = 2; i < self->width - 1; ++i)
      cigue_tty_puts(self->x + i, self->y, "─");

    for (int i = 1; i < self->height; ++i)
      cigue_tty_puts(self->x+1, self->y + i, "│");
  #ifdef CIGUE_GL
  }
  #endif
}

static void compute_size(cigue_state* s, cigue_widget* self) {

  //self_data* data = (self_data*) self->widget_data;

  assert(self->first_child && "Square root widget must have a child");
  assert(self->first_child->next == NULL && "Square root widget must have exactly one child");

  sqrt_layout lyt = get_layout(s);

  self->width = self->first_child->width + lyt.pref_size + lyt.suf_size;
  self->height = self->first_child->height + lyt.top_size;
  self->above_baseline = self->first_child->above_baseline + lyt.top_size;
}

void cigue_math_begin_sqrt(cigue_state *s) {

  assert(s != NULL && "Widget must be created in GUI. You passed gui = NULL.");

  cigue_widget* wgt = cigue_mem_new(s->buf, cigue_widget);
  self_data* data = cigue_mem_new(s->buf, self_data);

  wgt->widget_data = data;

  wgt->compute_dimensions = &compute_size;
  wgt->layout_and_draw = &layout_and_draw;

  cigue_begin(s, wgt); 
}
