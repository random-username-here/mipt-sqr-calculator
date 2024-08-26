#include <assert.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "glcvs/buffers.h"
#include "glcvs/context.h"
#include "istd/arr.h"

/// \brief Clean buffers, begin drawing new shape
void glcvs_buf_begin_new_shape(glcvs_ctx *ctx) {

  assert(ctx);

  ia_resize(ctx->_backend.vertices, 0);
  ia_resize(ctx->_backend.indices, 0);

  glUniform1i(ctx->_backend.shader.attr_use_texture, 0);
}

/// \brief Add new vertex to buffers
///
/// \returns Vertex index
unsigned glcvs_buf_add_vertex(glcvs_ctx* ctx, _glcvs_vertex vertex) {

  assert(ctx);

  vertex.pos.x = (vertex.pos.x / ctx->_width) * 2.0f - 1.0f;
  vertex.pos.y = 1.0f - (vertex.pos.y / ctx->_height) * 2.0f;

  ia_push(ctx->_backend.vertices, vertex);
  return ia_length(ctx->_backend.vertices) - 1;
}

static void _add_index(glcvs_ctx* ctx, unsigned idx) {
  
  assert(ctx);
  assert(idx < ia_length(ctx->_backend.vertices)); // must be a valid index

  ia_push(ctx->_backend.indices, idx);
}

/// \brief Add triangle to the given buffer
void glcvs_buf_add_triangle(glcvs_ctx* ctx, unsigned a, unsigned b, unsigned c) {
  
  _add_index(ctx, a);
  _add_index(ctx, b);
  _add_index(ctx, c);
}

/// \brief Draw buffers
/// Setting up uniforms for this is your job.
void glcvs_buf_draw(glcvs_ctx* ctx) {

  assert(ctx);

  glUseProgram(ctx->_backend.shader.program);
  glBindVertexArray(ctx->_backend.vertex_array);

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    ia_length(ctx->_backend.indices) * sizeof(unsigned),
    ctx->_backend.indices,
    GL_DYNAMIC_DRAW
  );

  glBufferData(
    GL_ARRAY_BUFFER,
    ia_length(ctx->_backend.vertices) * sizeof(_glcvs_vertex),
    ctx->_backend.vertices,
    GL_DYNAMIC_DRAW
  );

  glUniform4f(ctx->_backend.shader.attr_color, ctx->fill_color.r, ctx->fill_color.g, ctx->fill_color.b, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDrawElements(GL_TRIANGLES, ia_length(ctx->_backend.indices), GL_UNSIGNED_INT, 0);
}

