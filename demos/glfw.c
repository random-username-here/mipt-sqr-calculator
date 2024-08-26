// NOTE: glfw leaks some memory inside some
//       wayland-facing part
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "glcvs/gl-util.h"

static void fail(const char* msg) {
  printf("Failed: %s\n", msg);
}

static void gl_debug(GLenum source, GLenum type, GLuint id, GLenum severity,
              GLsizei length, const GLchar *message, const void *userParam) {
  printf("GL message: %s\n", message);
}

const char* vertex_shader = 
  "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "\n"
  "void main() {\n"
  "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
  "}\n";

const char* fragment_shader = 
  "#version 330 core\n"
  "out vec4 FragColor;\n"
  "\n"
  "void main() {\n"
  "  FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
  "}\n";

int main () {

  if (!glfwInit())
    fail("Failed to init GLFW");

  GLFWwindow* win = glfwCreateWindow(800, 600, "sqr-calculator", NULL, NULL);
  if (!win) {
    fail("Failed to create GLFW window");
    return 0;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  glfwMakeContextCurrent(win);

  if (glewInit() != GLEW_OK) {
    fail("Failed to init GLEW");
    glfwTerminate();
    return 0;
  }

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(&gl_debug, NULL);


  float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
  };

  unsigned array_buffer, vertex_array;
  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);

  glGenBuffers(1, &array_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(0);

  unsigned program = glcvs_make_program(vertex_shader, fragment_shader);
  if (program == GLCVS_PROGRAM_DID_NOT_COMPILE) {
    fail("Program did not compile!");
    glfwTerminate();
    return 0;
  }
  glUseProgram(program);
  
  glfwShowWindow(win);

  // ... setup

  while (!glfwWindowShouldClose(win)) {
    int w, h;
    glfwGetWindowSize(win, &w, &h);
    glViewport(0, 0, w, h);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vertex_array);
    glUseProgram(program);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // ... paint

    glfwPollEvents();
    glfwSwapBuffers(win);
  }

  glfwDestroyWindow(win);
  glfwTerminate();
}

