#include <string.h>
#include <stdio.h>

#ifdef GTK_UI
#include <gtk/gtk.h>
#include "sqe/ui.h"

void activate (GtkApplication* app, gpointer _) {
  GtkWindow* window = GTK_WINDOW(gtk_application_window_new(app));
  gtk_window_set_title(window, "Решатель квадратных уравнений");
  gtk_window_set_default_size(window, 800, 600);
  sqe_build_ui(window);
  gtk_window_present(window);
}


int gtk_main (int argc, char **argv) {
  GtkApplication* app = gtk_application_new("org.i-s-d.sqr-equation", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int rc = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return rc;
}
#endif

int main (int argc, char** argv) {
  if (argc > 1 && !strcmp(argv[1], "--help")) {
    fprintf(stderr, "sqe - Square equation solver\n\n");
    fprintf(stderr, "Variants:\n");
    fprintf(stderr, "  gtk - Graphical version, based on GTK4. Has its own help message.\n");
#ifndef GTK_UI
    fprintf(stderr, "        ! This version was compiled without -DGTK_UI, so this option does not work.\n");
#endif
    fprintf(stderr, "  curses - Ncurses-based version, runs by default.\n");
    return 0;
  }

  if (argc > 1 && !strcmp(argv[1], "gtk")) {
#ifdef GTK_UI
    // Прячем argv[1].
    argv[1] = argv[0];
    return gtk_main(argc-1, argv+1);
#else
    fprintf(stderr, "This executable was compiled without -DGTK_UI, so this won't work.");
    return -1;
#endif
  }

  if ((argc > 1 && !strcmp(argv[1], "curses")) || argc == 1) {
    printf("Curses gui here");
    return 0;
  }
  
  fprintf(stderr, "Wrong options, consider reading --help\n");
  return -1;
}
