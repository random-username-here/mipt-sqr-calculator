#include <assert.h>
#include "glcvs/drawing.h"
#include "glcvs/buffers.h"

void glcvs_fill_rect(glcvs_ctx *ctx, float x, float y, float w, float h) {

  assert(ctx);

  glcvs_buf_begin_new_shape(ctx);

  // a --- b
  // |     |
  // d --- c

  unsigned a = glcvs_buf_add_vertex(ctx, (_glcvs_vertex) { .pos = (glcvs_point){x, y} }),
           b = glcvs_buf_add_vertex(ctx, (_glcvs_vertex) { .pos = (glcvs_point){x+w, y} }),
           c = glcvs_buf_add_vertex(ctx, (_glcvs_vertex) { .pos = (glcvs_point){x+w, y+h} }),
           d = glcvs_buf_add_vertex(ctx, (_glcvs_vertex) { .pos = (glcvs_point){x, y+h} });

  glcvs_buf_add_triangle(ctx, a, b, c);
  glcvs_buf_add_triangle(ctx, c, d, a);

  glcvs_buf_draw(ctx);
}
