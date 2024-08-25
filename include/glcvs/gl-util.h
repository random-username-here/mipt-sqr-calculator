/**
 * \file Common wrappers for doing things with OpenGL
 */


#ifndef GLCVS_GL_UTIL
#define GLCVS_GL_UTIL

#include <GL/glew.h>
#include <GL/gl.h>

/// \brief Value returned when specified program
///        failed to compile/link
#define GLCVS_PROGRAM_DID_NOT_COMPILE ((GLuint) -1)

/// \brief Compile OpenGL shader
///
/// \param type   Shader type - commonly `GL_VERTEX_SHADER`
///               or `GL_FRAGMENT_SHADER`.
/// \param source C string, countaining source code for
///               that shader
///
/// \returns Shader handle or `GLCVS_PROGRAM_DID_NOT_COMPILE`
///          if something went wrong.
///
GLuint glcvs_compile_shader(GLenum type, const char* source);

/// \brief Make OpenGL shader out of vertex and fragment
///        sources.
///
GLuint glcvs_make_program(const char* vertex, const char* fragment);

#endif
