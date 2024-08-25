/**
 * \file
 * \brief Triangle-level drawing
 *
 * For cases when standard `draw_rect()` and such is not enough.
 */

#ifndef GLCVS_BUFFERS
#define GLCVS_BUFFERS

#include "glcvs/context.h"
#include "glcvs/geometry.h"

/// \brief Clean buffers, begin drawing new shape
void glcvs_buf_begin_new_shape(glcvs_ctx *ctx);

/// \brief Add new vertex to buffers
///
/// \returns Vertex index
unsigned glcvs_buf_add_vertex(glcvs_ctx* ctx, _glcvs_vertex vertex);

/// \brief Add triangle to the given buffer
void glcvs_buf_add_triangle(glcvs_ctx* ctx, unsigned a, unsigned b, unsigned c);

/// \brief Draw buffers
/// Setting up uniforms for this is your job.
void glcvs_buf_draw(glcvs_ctx* ctx);

#endif
