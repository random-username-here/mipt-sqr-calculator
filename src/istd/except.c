#include "istd/except.h"
#include <setjmp.h>
#include <stdio.h>

//---- Snow ball -----------------------------------------//

/// Snow ball of exceptions - or structure, which
/// stores a stack of exceptions.
/// Each exception on the stack has some other exception,
/// which was being handled while this happened.
struct exception_snowball {

  void* exception_data;
  iexcept_cleaner cleaner;
  iexcept_printer printer;

  /// Snow ball of exceptions being handled while this happened
  struct exception_snowball* while_handling;
};

typedef struct exception_snowball exception_snowball;

static exception_snowball* snowball = NULL;

/// Some other exception happened unhandled...
static void snowball_add_exception(
    void* data, iexcept_cleaner cleaner, iexcept_printer printer
  ) {
  exception_snowball* es =
    (exception_snowball*) istd_except_malloc(sizeof(exception_snowball));
  es->exception_data = data;
  es->cleaner = cleaner;
  es->printer = printer;
  es->while_handling = snowball;
  snowball = es;
}

/// We handled that snow ball, time to clean it
static void snowball_clean() {
  while (snowball) {
    exception_snowball* this_ball = snowball;
    snowball = snowball->while_handling;

    if (this_ball->cleaner)  
      this_ball->cleaner(this_ball->exception_data);
    
    istd_except_free(this_ball);
  }
}

//---- Catch tree ----------------------------------------//

struct catch_tree {
  struct catch_tree *parent;
  jmp_buf location;
};
typedef struct catch_tree catch_tree;

static catch_tree* jump_stack = NULL;

jmp_buf* _iexcept_catch_tree_new() {
  catch_tree* ni = (catch_tree*) istd_except_malloc(sizeof(catch_tree));
  ni->parent = jump_stack;
  jump_stack = ni;
  return &ni->location; 
}

void* _iexcept_thrown() {
  // Return only top exception
  return snowball ? snowball->exception_data : NULL;
}

void _iexcept_catch_tree_close() {
  // Clean exception handler frame
  catch_tree* this_frame = jump_stack;
  jump_stack = jump_stack->parent;
  istd_except_free(this_frame);
}

void _iexcept_clean_exc() {
  // Clean exception itself
  snowball_clean();
}

#define RED "\x1b[91m"
#define RESET "\x1b[0m"

void iexcept_throw(void* data, iexcept_cleaner clean, iexcept_printer print) {
  snowball_add_exception(data, clean, print);
  if (jump_stack) { // We are inside catch block, we have somewhere to jump to
    longjmp(jump_stack->location, 1);
  } else { // We are outside catch blocks, exception is not caught
    // Print uncaught
    fprintf(stderr, RED "\n\nUncaught exception:\n" RESET);
    if (snowball->printer)
      snowball->printer(snowball->exception_data);
    else
      fprintf(stderr, "<no printer specified for this exception>");
    // Print stack during handling of what this was raised
    exception_snowball* i = snowball->while_handling;
    while (i) {
      fprintf(stderr, RED "\nWhile handling exception:\n" RESET);
      if (i->printer)
        i->printer(i->exception_data);
      else
        fprintf(stderr, "<no printer specified for this exception>");
      i = i->while_handling;
    }
    // Free memory
    _iexcept_clean_exc();
  }
}

