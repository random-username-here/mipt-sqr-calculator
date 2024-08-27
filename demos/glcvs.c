// NOTE: glfw leaks some memory inside some
//       wayland-facing part
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "glcvs/geometry.h"
#include "glcvs/context.h"

#include "cigue/base.h"
#include "cigue/widgets.h"

#include "sqe/solve.h"
#include "sqe/cigue.h"

static void fail(const char* msg) {
  printf("Failed: %s\n", msg);
}

static void gl_debug(GLenum source, GLenum type, GLuint id, GLenum severity,
              GLsizei length, const GLchar *message, const void *userParam) {
  printf("GL message: %s\n", message);
}

static void text_input(cigue_state* gui, char* buf) {
  cigue_border(gui) {
    cigue_row(gui, 10) {
      cigue_label(gui, buf);
      //cigue_label(gui, "");
    }
  } 
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

  cigue_state* gui = cigue_new_state();
  cigue_use_glcvs(gui, ctx);
  
  glfwShowWindow(win);

  // ... setup

  while (!glfwWindowShouldClose(win)) {
    int w, h;
    glfwGetWindowSize(win, &w, &h);
    glViewport(0, 0, w, h);
    glcvs_context_resize(ctx, (float) w, (float) h);

    glcvs_color bg = CIGUE_BACKGROUND;
    glClearColor(bg.r, bg.g, bg.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /*ctx->fill_color = (glcvs_color) { 1.0f, 0.5f, 0.0f };
    glcvs_fill_rect(ctx, 20, 20, 30, 10);

    ctx->fill_color = (glcvs_color) { 0.5f, 0.5f, 0.5f };
    glcvs_fill_rect(ctx, 20, 40, 30, 10);

    ctx->fill_color = (glcvs_color) { 0.2f, 0.2f, 0.2f };
    glcvs_fill_rect(ctx, 20, 80, 100, 20);

    glcvs_fill_text(ctx, 20, 60, "Hello world!");
    glcvs_fill_text(ctx, 20, 80, "More text!");
    glcvs_fill_text(ctx, 20, 100, "Юникод работает.");*/
    ctx->fill_color = (glcvs_color) { 1.0f, 1.0f, 1.0f };

    cigue_column(gui, 10) {
      cigue_label(gui, "Решатель квадратных уравнений");
      cigue_label(gui, "На данный момент интерактивности нет, это просто тест cigue");

      cigue_row(gui, 5) {
        text_input(gui, "1");
        cigue_label(gui, "x² + ");
        text_input(gui, "4");
        cigue_label(gui, "x + ");
        text_input(gui, "3");
        cigue_label(gui, " = 0");
      }

      solve_result sr = sqe_solve(1, 4, 3);
      sqe_cigue_paint_solution(gui, &sr);
    }

    cigue_render(20, 20, gui);

    glfwPollEvents();
    glfwSwapBuffers(win);
  }

  cigue_free_state(gui);
  glcvs_context_destroy(ctx);

  glfwDestroyWindow(win);
  glfwTerminate();
}

