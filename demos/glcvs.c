// NOTE: glfw leaks some memory inside some
//       wayland-facing part
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "glcvs/drawing.h"
#include "glcvs/geometry.h"
#include "glcvs/gl-util.h"
#include "glcvs/context.h"
#include "glcvs/drawing.h"

static void fail(const char* msg) {
  printf("Failed: %s\n", msg);
}

static void gl_debug(GLenum source, GLenum type, GLuint id, GLenum severity,
              GLsizei length, const GLchar *message, const void *userParam) {
  printf("GL message: %s\n", message);
}

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

  glcvs_ctx* ctx = glcvs_context_new();
  if (!ctx) {
    fail("Glcvs contest not created");
    glfwTerminate();
    return 0;
  }
  
  glfwShowWindow(win);

  // ... setup

  while (!glfwWindowShouldClose(win)) {
    int w, h;
    glfwGetWindowSize(win, &w, &h);
    glViewport(0, 0, w, h);
    glcvs_context_resize(ctx, w, h);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ctx->fill_color = (glcvs_color) { 1.0f, 0.5f, 0.0f };
    glcvs_fill_rect(ctx, 20, 20, 30, 10);

    ctx->fill_color = (glcvs_color) { 0.5f, 0.5f, 0.5f };
    glcvs_fill_rect(ctx, 20, 40, 30, 10);

    glfwPollEvents();
    glfwSwapBuffers(win);
  }

  glcvs_context_destroy(ctx);

  glfwDestroyWindow(win);
  glfwTerminate();
}

