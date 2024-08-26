#ifndef GLCVS_CONTEXT
#define GLCVS_CONTEXT

#include <stddef.h>
#include "istd/arr.h"
#include "glcvs/geometry.h"

/// Vertex buffer entry
typedef struct {
  glcvs_point pos;
  float z;
  glcvs_point uv;
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

    /// Fill color of the drawn shape (text is also tinted with this color)
    /// Attribute type is vec4.
    int attr_color;

    /// Texture sampler. 
    int attr_texture;

    /// Use texture sampler or not? Boolean
    int attr_use_texture;
  } shader;

  /// Drawing buffers
  unsigned array_buffer, element_buffer;

  /// Vertex array with those buffers
  unsigned vertex_array;

  /// Font texture handler.
  unsigned font_atlas_tex;

} _glcvs_backend_data;

/// \brief Context used by glcvs
typedef struct {

  /// Data related to OpenGL
  _glcvs_backend_data _backend;

  /// Size of the viewport
  float _width, _height;

  /// Current fill color (also applies to text)
  glcvs_color fill_color;
} glcvs_ctx;

/// \brief Create glcvs context
/// \returns Yout new context or `NULL` if something went wrong.
glcvs_ctx* glcvs_context_new();

/// \brief Size of the window changed
void glcvs_context_resize(glcvs_ctx* ctx, float w, float h);

/// \brief Free context you no longer need
void glcvs_context_destroy(glcvs_ctx* ctx);


#endif // GlCVS_CONTEXT
