#ifndef GLCVS_CONTEXT
#define GLCVS_CONTEXT

#include <stddef.h>
#include "istd/arr.h"
#include "glcvs/geometry.h"

/// Vertex buffer entry
typedef struct {
  glcvs_point pos;
  float z;

} _glcvs_vertex;


typedef struct {
  /// Vertices of current path.
  ia_arr(_glcvs_vertex) vertices;

  /// Vertex indices of this path
  ia_arr(unsigned) indices;

  /// Shader and information about it
  struct {
    /// Shader to render common objects
    /// Should support simple linear gradients
    unsigned program;

    unsigned attr_color;
  } shader;

  unsigned array_buffer, element_buffer;

  unsigned vertex_array;

} _glcvs_backend_data;

/// \brief Context used by glcvs
typedef struct {
  _glcvs_backend_data _backend;
  float _width, _height;
} glcvs_ctx;

/// \brief Create glcvs context
/// \returns Yout new context or `NULL` if something went wrong.
glcvs_ctx* glcvs_context_new();

/// \brief Size of the window changed
void glcvs_context_resize(glcvs_ctx* ctx, float w, float h);

/// \brief Free context you no longer need
void glcvs_context_destroy(glcvs_ctx* ctx);


#endif // GlCVS_CONTEXT
