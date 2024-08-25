#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "glcvs/gl-util.h"

#define INFO_LOG_BUF_SIZE 1024

/// Compile given shader
GLuint glcvs_compile_shader(GLenum type, const char* code) {

  assert(code);

  // Create new shader
  GLuint shader = glCreateShader(type);

  // Attach source to it
  GLint code_len = (GLint) strlen(code);
  glShaderSource(shader, 1, &code, &code_len);

  // Compile
  glCompileShader(shader);

  // Check for errors
  GLint res;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
  if (res != GL_TRUE) { // There were errors
    char info[INFO_LOG_BUF_SIZE];
    glGetShaderInfoLog(shader, INFO_LOG_BUF_SIZE, NULL, info);
    fprintf(stderr, "Failed to compile %s shader:\n%s", (type == GL_FRAGMENT_SHADER ? "fragment" : "vertex"), info);
    return GLCVS_PROGRAM_DID_NOT_COMPILE;
  }

  // Done
  return shader;
}

/// Construct program out of vertex and fragment shader
/// sources.
GLuint glcvs_make_program(const char* vertex, const char* fragment) {

  assert(fragment);
  assert(vertex);

  // Compile shaders
  GLuint fragment_shader = glcvs_compile_shader(GL_FRAGMENT_SHADER, fragment),
           vertex_shader = glcvs_compile_shader(GL_VERTEX_SHADER, vertex);

  // Check what they were compiled
  if (fragment_shader == GLCVS_PROGRAM_DID_NOT_COMPILE ||
      vertex_shader == GLCVS_PROGRAM_DID_NOT_COMPILE)
    return GLCVS_PROGRAM_DID_NOT_COMPILE;

  // Assemble program
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  // Check for link errors
  GLint res;
  glGetProgramiv(program, GL_LINK_STATUS, &res);
  if (res != GL_TRUE) {
    char info[INFO_LOG_BUF_SIZE];
    glGetProgramInfoLog(program, INFO_LOG_BUF_SIZE, NULL, info);
    fprintf(stderr, "Failed to compile shader:\n%s", info);
    return GLCVS_PROGRAM_DID_NOT_COMPILE;
  }

  // Remove no longer needed shader data
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // We are done
  return program;
}

