#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include "sqe/cigue.h"
#ifdef GTK_UI
  #include "sqe/ui.h"
#endif

static void print_help() {
  printf("sqe - Square equation solver\n\n");
  printf("Variants:\n");
  printf("  gtk - Graphical version, based on GTK4. Has its own help message.\n");
  #ifndef GTK_UI
    printf("        ! This version was compiled without -DGTK_UI, so this option does not work.\n");
  #endif
  printf("  cigue Cigue-based version, runs by default.\n");
}

int main (int argc, char** argv) {
  if (argc > 1 && !strcmp(argv[1], "--help")) {
    print_help();
    return EXIT_SUCCESS;
  }

  if (argc > 1 && !strcmp(argv[1], "gtk")) {
    #ifdef GTK_UI
      // Прячем argv[1].
      argv[1] = argv[0];
      return gtk_main(argc-1, argv+1);
    #else
      printf("This executable was compiled without -DGTK_UI, so this won't work.");
      return EXIT_FAILURE;
    #endif
  }

  if ((argc > 1 && !strcmp(argv[1], "cigue")) || argc == 1) {
    sqe_cigue_ui();
    return EXIT_SUCCESS;
  }
  
  printf("Wrong options, consider reading --help\n");
  return EXIT_FAILURE;
}
