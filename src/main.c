#include <gtk/gtk.h>
#include "glib.h"
#include "sqe/ui.h"

void activate (GtkApplication* app, gpointer _) {
  GtkWindow* window = GTK_WINDOW(gtk_application_window_new(app));
  gtk_window_set_title(window, "Решатель квадратных уравнений");
  gtk_window_set_default_size(window, 800, 600);
  sqe_build_ui(window);
  gtk_window_present(window);
}

int main (int argc, char **argv) {
  GtkApplication* app = gtk_application_new("org.i-s-d.sqr-equation", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int rc = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return rc;
}
