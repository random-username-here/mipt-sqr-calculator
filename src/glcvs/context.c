#include <assert.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <time.h>
#include "istd/arr.h"
#include "glcvs/context.h"
#include "glcvs/gl-util.h"

/// Size to use until user set context size
/// (if this was 0, we would get division error and nothing shown)
#define CTX_TEMP_SIZE 100

const char* vertex_shader = 
  "#version 330 core\n"
  "layout (location = 0) in vec2 aPos;\n"
  "layout (location = 1) in float aZ;\n"
  "\n"
  "void main() {\n"
  "  gl_Position = vec4(aPos.x, aPos.y, aZ, 1.0);\n"
  "}\n";

const char* fragment_shader = 
  "#version 330 core\n"
  "out vec4 FragColor;\n"
  "uniform vec4 color;\n"
  "\n"
  "void main() {\n"
  "  FragColor = color;\n"
  "}\n";


glcvs_ctx* glcvs_context_new() {
  glcvs_ctx* ctx = calloc(1, sizeof(glcvs_ctx));
  ctx->_width = ctx->_height = CTX_TEMP_SIZE;

  ctx->_backend.vertices = ia_empty_array(_glcvs_vertex);
  ctx->_backend.indices = ia_empty_array(unsigned);

  unsigned shader = ctx->_backend.shader.program = glcvs_make_program(vertex_shader, fragment_shader);
  if (shader == GLCVS_PROGRAM_DID_NOT_COMPILE)
    return NULL;

  ctx->_backend.shader.attr_color = glGetUniformLocation(shader, "color");

  glGenVertexArrays(1, &ctx->_backend.vertex_array);
  glBindVertexArray(ctx->_backend.vertex_array);

  glGenBuffers(1, &ctx->_backend.array_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, ctx->_backend.array_buffer); 

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(_glcvs_vertex), (void*) offsetof(_glcvs_vertex, pos));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(_glcvs_vertex), (void*) offsetof(_glcvs_vertex, z));
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &ctx->_backend.element_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->_backend.element_buffer);

  return ctx;
}

void glcvs_context_resize(glcvs_ctx* ctx, float w, float h) {

  assert(ctx);

  ctx->_width = w;
  ctx->_height = h;
}

void glcvs_context_destroy(glcvs_ctx* ctx) {

  assert(ctx);

  ia_destroy(ctx->_backend.vertices);
  ia_destroy(ctx->_backend.indices);
  free(ctx);
}
